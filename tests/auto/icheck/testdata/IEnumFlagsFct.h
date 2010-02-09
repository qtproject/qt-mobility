class IEnumFlags 
{
public:
    Q_ENUMS(Enum1 Enum2 Enum3)
    enum Enum1 {
        E10 = 0,
        E11,
        E12,
        E13
    };

    enum Enum2 {
        E20 = 0,
        E21,
        E22,
        E23
    };

    enum Enum3 {
        E30 = 0,
        E31,
        E32,
        E33
    };

    //using wrong flag name
    Q_FLAGS(EnumF1 EnumF2 EnumF3)
    enum EnumF1 {
        EF10 = 0,
        EF11,
        EF12,
        EF13
    };

    enum EnumF2 {
        EF20 = 0,
        EF21,
        EF22,
        EF23
    };

    enum EnumF3 {
        EF30 = 0,
        EF31,
        EF32,
        EF33
    };

    enum EnumDF1 {
        DF10 = 0,
        DF11,
        DF12,
        DF13
    };

    enum EnumDF2 {
        DF20 = 0,
        DF21,
        DF22,
        DF23
    };
    Q_DECLARE_FLAGS(FlagsDF1, EnumDF1)
    Q_DECLARE_FLAGS(FlagsDF2, EnumDF2)
    Q_FLAGS(FlagsDF1 FlagsDF2)

    //this enum is implemented in QFrame
    Q_ENUMS(Shape)
    enum Shape{
        NoFrame = 0,
        Box = 0x0001,
        Panel = 0x0002,
        StyledPanel = 0x0006,
        HLine = 0x0004,
        VLine = 0x0005,
        WinPanel = 0x0003
    };
};
