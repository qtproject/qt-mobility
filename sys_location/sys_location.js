

testcase = {
    initTestCase: function()
    {
    },

    current_position: function()
    {
        prompt(twiki('---+++ Position is detected
    1. Run the weatherinfo_with_location example (sources are in examples/weatherinfo).
    1. Verify that weather information for your local area is displayed.
        '));
    },

    location_is_updated: function()
    {
        prompt(twiki('---+++ Position changes are detected
    1. Run the fetchgooglemaps example (sources are in examples/fetchgooglemaps).
    1. Note the initial position displayed in the example.
    1. Move to a different location.
    1. Note the updated position displayed in the example.
    1. Verify that the new position in step 4 corresponds to the move in step 3 (i.e. if the device moved north, the new position should clearly indicate this).
       '));
    }
    // *** Start of Test Functions ***

}  // end of testcase

