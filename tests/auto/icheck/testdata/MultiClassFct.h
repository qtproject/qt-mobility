class IInterface0Impl
{
signals:
    void Signal_IInterface0_1();
    void Signal_IInterface0_2();

protected slots:
    void Slot_IInterface0_1();
    void Slot_IInterface0_2();
};

class IInheritInterface0Impl : public IInterface3Impl
{
signals:
    void Signal_IInterface0_1();
    void Signal_IInterface0_2();
    void Signal_IInterface1_1();
    void Signal_IInterface1_2();

    void Signal_IInheritInterface0_1();
protected slots:
    void Slot_IInterface0_1();
    void Slot_IInterface0_2();
    void Slot_IInterface1_1();
    void Slot_IInterface1_2();

    void Slot_IInheritInterface0_1();
};

class IInterface3Impl
{
signals:
    void Signal_IInterface3_1();
    void Signal_IInterface3_2();

protected slots:
    void Slot_IInterface3_1();
    void Slot_IInterface3_2();
};