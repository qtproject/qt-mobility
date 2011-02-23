function changeDate() {
   //TODO
}

function changeToday() {
    yearList.currentIndex = timelineView.year - yearModel.start;
    monthList.currentIndex = timelineView.month;
    dayList.positionViewAtIndex(timelineView.day, ListView.Center);
    dayList.currentIndex = timelineView.day;
}
function extendYearModel(init) {

    var start = yearModel.start;
    var end = yearModel.end;
    var now = new Date();
    var year = 1900 + now.getYear();

    if (init) {
        //initializes the year model
        if (yearModel.count == 1) {
            yearModel.set(0, {"year" : year});
            start = year;
            end = year;
        }
    }

    if (start == 0) return;

    //extends forward
    if (yearList.currentIndex == yearList.count - 1) {
        for (var i = 0; i < 10; i ++) {
            end++;
            yearModel.append({"year" : end});
        }
    }


    //extends backward
    if (yearList.currentIndex == 0) {
        if (start == 1900)
             break;
        for (var i = 0; i < 10; i ++) {
            start--;
            yearModel.insert(1, {"year" : start});
        }
        yearModel.move(0, 10, 1);
    }
    yearModel.start = start;
    yearModel.end = end;
    if (init) {
        yearList.currentIndex = year - start;
    }
}
