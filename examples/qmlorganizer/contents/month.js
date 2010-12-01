
Date.prototype.clone = function() { return new Date(this.getTime()); }

function getDayOfMonth(startDayOfMonth, offset) {
    var day = startDayOfMonth.clone();
    day.setDate(offset);
    return day.getDate();
}


function getColorOfDay(startDayOfMonth, offset) {

    var newDay = startDayOfMonth.clone();
    newDay.setDate(offset);
//    console.log("startDay:" + startDayOfMonth);
//    console.log("offset:" + offset);
//    console.log("newDay:" + newDay);
    if (newDay.getMonth() == startDayOfMonth.getMonth())
        return "white";
    else
        return "lightgray";

}
