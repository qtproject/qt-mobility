class IMyInterface 
{
    Q_ENUMS(PropertyEnum_1 propertyenum1)
    Q_PROPERTY( PropertyEnum_1 propName_1 READ propRead_1 WRITE propWrite_1 RESET propReset_1 NOTIFY propNotify_1);
public:
    PropertyEnum_1 propRead_1() const;
    void propWrite_1( const PropertyEnum_1& value);
    void propReset_1();
    void propNotify_1();
    enum propertyenum1 {
        prop0 = 0,
        prop1,
        prop2,
        prop3
    };
};
