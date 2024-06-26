import argparse
import lz4.block
import os.path
import png
import stringcase


def alpha_blending(intensity, alpha):
    intensity_double = intensity / 255.0
    # Assume a white background (1.0, 1.0, 1.0) in the blending
    intensity_double = intensity_double * alpha + (1.0 - alpha) * 1.0
    return int(intensity_double * 255.0)


def rgba8882rgb565(red, green, blue, alpha):
    alpha = alpha / 255.0
    r, g, b = (
        alpha_blending(red, alpha),
        alpha_blending(green, alpha),
        alpha_blending(blue, alpha),
    )
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)


def generate_c_files(png_file, header_file_name, cimplementation_file_name, verbose):
    png_reader = png.Reader(filename=png_file)
    width, height, data, info = png_reader.asRGBA8()

    # Remove path prefix
    png_file = os.path.basename(png_file)
    # Remove path extension
    png_file = os.path.splitext(png_file)[0]
    png_name_snake_case = stringcase.snakecase(png_file)
    png_name_upper_snake_case = stringcase.uppercase(png_name_snake_case)
    png_name_camel_case = stringcase.capitalcase(stringcase.camelcase(png_file))

    # Convert RGBA888 to RGB565
    dataRGB565 = []
    for row in data:
        for i in range(0, len(row), 4):
            r, g, b, a = row[i], row[i + 1], row[i + 2], row[i + 3]
            dataRGB565.extend(rgba8882rgb565(r, g, b, a).to_bytes(2, "little"))
    # Compress data
    compressed_data = lz4.block.compress(
        bytes(dataRGB565), compression=12, mode="high_compression", store_size=False
    )
    compressed_data_len = len(compressed_data)

    # Generate header file
    header_file = open(header_file_name, "w")
    header_file.write(
        "// This file is auto-generated by PNG serializer. Do not edit manually.\n"
    )
    header_file.write("#ifndef " + png_name_upper_snake_case + "_H\n")
    header_file.write("#define " + png_name_upper_snake_case + "_H\n\n")
    header_file.write("#include <stdint.h>\n\n")
    header_file.write("namespace Ion {\n")
    header_file.write("namespace " + png_name_camel_case + " {\n\n")
    header_file.write(
        "constexpr uint32_t k_compressedPixelSize = "
        + str(compressed_data_len)
        + ";\n\n"
    )
    header_file.write("constexpr uint32_t k_width = " + str(width) + ";\n\n")
    header_file.write("constexpr uint32_t k_height = " + str(height) + ";\n\n")
    header_file.write("extern const uint8_t compressedPixelData[];\n\n")
    header_file.write("}\n")
    header_file.write("}\n")
    header_file.write("#endif\n")
    header_file.close()

    # Generate cimplementation file
    cimplementation_file = open(cimplementation_file_name, "w")
    cimplementation_file.write(
        "// This file is auto-generated by Inliner. Do not edit manually.\n"
    )
    cimplementation_file.write('#include "' + header_file_name + '"\n\n')
    cimplementation_file.write("namespace Ion {\n")
    cimplementation_file.write("namespace " + png_name_camel_case + " {\n\n")
    cimplementation_file.write(
        "// Compressed "
        + str(width * height)
        + " pixels into "
        + str(compressed_data_len)
        + " bytes ("
        + str(100.0 * compressed_data_len / len(dataRGB565))
        + "% compression ratio)\n\n"
    )
    cimplementation_file.write(
        "const uint8_t compressedPixelData[" + str(compressed_data_len) + "] = {"
    )
    for b in compressed_data:
        cimplementation_file.write(hex(b) + ", ")
    cimplementation_file.write("\n};\n\n")
    cimplementation_file.write("}\n")
    cimplementation_file.write("}\n")
    cimplementation_file.close()


parser = argparse.ArgumentParser(
    description="Extract data from png, inline them in C code in a compressed (LZ4) format."
)
parser.add_argument("--png", metavar="PNG_FILE", help="Input PNG file")
parser.add_argument("--header", metavar="HEADER_FILE", help="Output C header file")
parser.add_argument("--cimplementation", metavar="C_FILE", help="Output C header file")
parser.add_argument("-v", "--verbose", action="store_true", help="Show verbose output")

args = parser.parse_args()
generate_c_files(args.png, args.header, args.cimplementation, args.verbose)
