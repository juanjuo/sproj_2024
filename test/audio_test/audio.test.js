// import {AppConnection} from '@focusritegroup/juce-end-to-end';

const {AppConnection} = require('@focusritegroup/juce-end-to-end');
//const SECONDS = 1000;

it('test_1', async () =>{
    const app = new AppConnection({appPath: "/Users/juan/Desktop/C-C++/Sproj2024/cmake-build-debug/sproj_2024/sproj2024_artefacts/Debug/Sproj2024.app/Contents/MacOS/Sproj2024"});
    await app.launch();
    await app.quit();
});


// describe('My app tests', () => {
//     let appConnection: AppConnection;
//
//     beforeEach(async () => {
//         appConnection = new AppConnection({appPath: '/Users/juan/Desktop/C-C++/Sproj2024/cmake-build-debug/sproj_2024/sproj2024_artefacts/Debug/Sproj2024'});
//         await appConnection.launch();
//     });
//
//     afterEach(async () => {
//         await appConnection.quit();
//     });
// })




