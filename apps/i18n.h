#ifndef APPS_I18N_H
#define APPS_I18N_H

#include <escher.h>

namespace I18n {
  enum class Message : uint16_t {
    Warning = 0,
    Ok = 1,
    Next = 2,
    SyntaxError,
    MathError,

    /* Variables */
    Variables,
    Number,
    Matrix,
    List,

    /* Toolbox */
    Toolbox,
    AbsoluteValue,
    NthRoot,
    BasedLogarithm,
    Calculation,
    ComplexNumber,
    Probability,
    Arithmetic,
    Matrices,
    Lists,
    Approximation,
    HyperbolicTrigonometry,
    Fluctuation,
    DerivateNumber,
    Integral,
    Sum,
    Product,
    ComplexAbsoluteValue,
    Agument,
    ReelPart,
    ImaginaryPart,
    Conjugate,
    Combination,
    Permutation,
    GreatCommonDivisor,
    LeastCommonMultiple,
    Remainder,
    Quotient,
    Inverse,
    Determinant,
    Transpose,
    Trace,
    Dimension,
    Sort,
    InvSort,
    Maximum,
    Minimum,
    Floor,
    FracPart,
    Ceiling,
    Rounding,
    Prediction95,
    Prediction,
    Confidence,

    /* Applications */
    Apps,
    AppsCapital,

    /* Calculation */
    CalculApp,
    CalculAppCapital,

    /* Fonction */
    FunctionApp,
    FunctionAppCapital,
    FunctionTab,
    GraphTab,
    ValuesTab,
    Plot,
    DisplayValues,
    FunctionOptions,
    AddFunction,
    DeleteFunction,
    NoFunctionToDelete,
    ActivateDesactivate,
    FunctionColor,
    NoFunction,
    NoActivatedFunction,
    Axis,
    Zoom,
    InteractiveZoom,
    PredefinedZoom,
    Initialization,
    Move,
    ToZoom,
    Or,
    Trigonometric,
    RoundAbscissa,
    Orthonormal,
    DefaultSetting,
    PlotOptions,
    Compute,
    Goto,
    Zeros,
    Tangent,
    Intersection,
    SelectLowerBound,
    SelectUpperBound,
    NoZeroFound,
    IntervalSet,
    XStart,
    XEnd,
    XStep,
    XColumn,
    FunctionColumn,
    DerivativeColumn,
    DerivativeFunctionColumn,
    ClearColumn,
    CopyColumnInList,
    HideDerivativeColumn,

    /* Sequence */
    SequenceApp,
    SequenceAppCapital,
    SequenceTab,
    AddSequence,
    ChooseSequenceType,
    SequenceType,
    Explicite,
    SingleRecurrence,
    DoubleRecurrence,
    SequenceOptions,
    SequenceColor,
    DeleteSequence,
    NoSequence,
    NoActivatedSequence,
    TermSum,
    SelectFirstTerm,
    SelectLastTerm,
    NColumn,

  /* Statistics */
    StatsApp,
    StatsAppCapital,
    DataTab,
    HistogramTab,
    BoxTab,
    StatTab,
    Values,
    Sizes,
    ColumnOptions,
    ImportList,
    NoDataToPlot,
    Interval,
    Size,
    Frequency,
    HistogramSet,
    RectangleWidth,
    BarStart,
    FirstQuartile,
    Median,
    ThirdQuartile,
    NoValueToCompute,
    TotalSize,
    Range,
    Mean,
    StandardDeviation,
    Deviation,
    InterquartileRange,
    SquareSum,

    /* Probability  */
    ProbaApp,
    ProbaAppCapital,
    ChooseLaw,
    Binomial,
    Uniforme,
    Exponential,
    Normal,
    Poisson,
    BinomialLaw,
    UniformLaw,
    ExponentialLaw,
    NormalLaw,
    PoissonLaw,
    ChooseParameters,
    RepetitionNumber,
    SuccessProbability,
    IntervalDefinition,
    LambdaExponentialDefinition,
    MeanDefinition,
    DeviationDefinition,
    LambdaPoissonDefinition,
    ComputeProbability,
    ForbiddenValue,
    UndefinedValue,

    /* Regression */
    RegressionApp,
    RegressionAppCapital,
    NoEnoughDataForRegression,
    RegressionSlope,
    XPrediction,
    YPrediction,
    ValueNotReachedByRegression,
    NumberOfDots,
    Covariance,

    /* Settings */
    SettingsApp,
    SettingsAppCapital,
    AngleUnit,
    DisplayMode,
    ComplexFormat,
    Language,
    Degres,
    Radian,
    Auto,
    Scientific,
    Deg,
    Rad,
    Sci,

    /* UNIVERSAL MESSAGES */
    Default = 0x8000,
    X,
    Y,
    N,
    P,
    Mu,
    Sigma,
    Lambda,
    A,
    B,
    R,
    Sxy,

    Cosh,
    Sinh,
    Tanh,
    Acosh,
    Asinh,
    Atanh,

    XMin,
    XMax,
    YMin,
    YMax,
    YAuto,

    RightIntegralFirstLegend,
    RightIntegralSecondLegend,
    LeftIntegralFirstLegend,
    LeftIntegralSecondLegend,
    FiniteIntegralLegend,

    RegressionFormula,

    French,
    English,
    Spanish,

    /* Toolbox: commands */
    AbsCommand,
    RootCommand,
    LogCommand,
    DiffCommand,
    IntCommand,
    SumCommand,
    ProductCommand,
    ArgCommand,
    ReCommand,
    ImCommand,
    ConjCommand,
    BinomialCommand,
    PermuteCommand,
    GcdCommand,
    LcmCommand,
    RemCommand,
    QuoCommand,
    InverseCommand,
    DeterminantCommand,
    TransposeCommand,
    TraceCommand,
    DimensionCommand,
    SortCommand,
    InvSortCommand,
    MaxCommand,
    MinCommand,
    FloorCommand,
    FracCommand,
    CeilCommand,
    RoundCommand,
    CoshCommand,
    SinhCommand,
    TanhCommand,
    AcoshCommand,
    AsinhCommand,
    AtanhCommand,
    Prediction95Command,
    PredictionCommand,
    ConfidenceCommand,
  };
  enum class Language : uint16_t {
    Default = 0,
    French = 1,
    English = 2,
    Spanish = 3
  };
}

#endif

