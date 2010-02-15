class IInterface0
{
signals:
    void Signal_IInterface0_1();
    void Signal_IInterface0_2();

protected slots:
    void Slot_IInterface0_1();
    void Slot_IInterface0_2();
};

class IInterface1
{
signals:
    void Signal_IInterface1_1();
    void Signal_IInterface1_2();

protected slots:
    void Slot_IInterface1_1();
    void Slot_IInterface1_2();
};

class IInheritInterface0 : public IInterface0
{
signals:
    void Signal_IInheritInterface0_1();
protected slots:
    void Slot_IInheritInterface0_1();
};

class IInterface3
{
signals:
    void Signal_IInterface3_1();
    void Signal_IInterface3_2();

protected slots:
    void Slot_IInterface3_1();
    void Slot_IInterface3_2();
};