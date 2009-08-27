import Qt 4.6
import Example 1.0

Rectangle {
    id: Page
    color: "White"

    Rectangle {
        id: Terminal
        x: 20
        y: 10
        width: 60
        height: 10
        color: "Black"
    }

    Rectangle {
        id: Battery
        x: 10
        y: 20
        width: 80
        height: 200
        color: "Black"
    }

    Rectangle {
        id: VisualCharge
        x: 12
        y: 22 + 196 - height
        width: 76
        height: 196 * Charge.charge / 100
        color: "Green"
    }

    BatteryCharge {
        id: Charge
        path: "/power/battery/charge"
    }
}
