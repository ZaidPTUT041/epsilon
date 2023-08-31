import sys, os, shutil, subprocess, argparse
from pathlib import Path
import helper
import args_types

parser = argparse.ArgumentParser(description='This script takes a screenshot for each event of a scenario and creates a gif.')
parser.add_argument('state_file', metavar='STATE_FILE', type=args_types.existing_state_file, help='state file (with extension .nws)')
parser.add_argument('-o', '--output_folder', default='screenshots_each_step', help='output folder')

def main():
   # Parse args
   args = parser.parse_args()

   # Clean or create output folder
   if os.path.exists(args.output_folder):
      print("Warning:", args.output_folder, "already exists. Its content will be erased.")
      shutil.rmtree(args.output_folder)
   os.mkdir(args.output_folder)

   # Generate the screenshots
   executable = helper.executable_built_path()
   helper.generate_all_screenshots(args.state_file, executable, args.output_folder)
   list_images = [image.as_posix() for image in sorted(Path(args.output_folder).glob("*.png"))]
   if len(list_images) == 0:
      print("Error: couldn't take screenshots")
      sys.exit(1)
   print("All done, screenshots taken in", args.output_folder)

   # Create gif
   print("Creating gif")
   gif = os.path.join(args.output_folder, "scenario.gif")
   subprocess.run("convert -set delay '%[fx:t==(n-1) ? 175 : 35]' " + ' '.join(list_images) + " " + gif, shell=True)
   if not os.path.exists(gif):
      print("Error: couldn't create gif")

if __name__ == "__main__":
    main()
