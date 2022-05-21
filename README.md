# esp32-spotify-oled
View your current playing song on ESP32 &amp; OLED display using Spotify API

### Libraries
- [`OLED_I2C`](http://www.rinkydinkelectronics.com/library.php?id=79)


### Spotify
- Visit Spotify Dashboard [`https://developer.spotify.com/dashboard/login`](https://developer.spotify.com/dashboard/login)
- [`Create an app`](https://developer.spotify.com/documentation/general/guides/authorization/app-settings/)
- Copy Client ID to sketch
- Add Redirect URI http://httpbin.org/anything & Save in `Edit Settings`

### Authentication
- Authenticate the app using your browser (Chrome)
- Enable Developer tools `Ctrl+Shift+I`
- Add Client ID & Redirect URI (URL Encoded) -> `http%3A%2F%2Fhttpbin.org%2Fanything`
- Authentication Link: `https://accounts.spotify.com/authorize?response_type=token&redirect_uri=" + redirect + "&client_id=" + client_id + "&scope=user-read-playback-state+user-read-playback-position+user-modify-playback-state&state=cryq3`
- It should redirect to the specified URI with an Access Token on the URL
- Copy the Cookie value to the sketch

![image](images/cookie.jpg?raw=true "image")
