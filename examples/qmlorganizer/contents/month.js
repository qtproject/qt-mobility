
Date.prototype.clone = function() { return new Date(this.getTime()); }

function getDayOfMonth(startDayOfMonth, offset) {
    var day = startDayOfMonth.clone();
    day.setDate(offset);
    return day.getDate();
}

function dateOfThisDay(startDayOfMonth, offset) {
    var day = startDayOfMonth.clone();
    day.setDate(offset);
    return day;
}

function getColorOfDay(startDayOfMonth, offset) {

    var newDay = startDayOfMonth.clone();
    newDay.setDate(offset);
    if (newDay.getMonth() == startDayOfMonth.getMonth())
        return "white";
    else
        return "lightgray";

}

function isToday(startDayOfMonth, offset) {

    var newDay = startDayOfMonth.clone();
    newDay.setDate(offset);
    var today = new Date();
    return newDay.toDateString() == today.toDateString();
}

