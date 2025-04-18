// import {AppConnection} from '@focusritegroup/juce-end-to-end';

const {AppConnection} = require('@focusritegroup/juce-end-to-end');
const SECONDS = 1000;

const app = new AppConnection({appPath: "/Users/juan/Desktop/C-C++/Sproj2024/cmake-build-debug/sproj_2024/sproj2024_artefacts/Debug/Sproj2024.app/Contents/MacOS/Sproj2024"});

beforeEach(async () => {
    await app.launch();
});

afterEach(async () => {
    await app.quit();
}, 10 * SECONDS);

it('metronome_gain', async () =>{
    await app.setSliderValue("test_metronome_volume_slider", 2);
    const value = await app.getSliderValue("test_metronome_volume_slider")
    expect(value).toEqual(2);
});

it('track_gain', async () =>{
    await app.setSliderValue("test_track_volume_slider", 2);
    const value = await app.getSliderValue("test_track_volume_slider")
    expect(value).toEqual(2);
});

// it('component_focus', async () =>{
//     const component = await app.getFocusedComponent()
//     expect(component).toEqual("test_mask");
// });

it('command_manager_menu', async () =>{
    let isActive = await app.getComponentVisibility("test_deviceSelector")
    expect(isActive).toEqual(false);

    await app.keyPress("l", "command")
    isActive = await app.getComponentVisibility("test_deviceSelector")
    expect(isActive).toEqual(true);
});

it('command_manager_start_processing', async () =>{

});







