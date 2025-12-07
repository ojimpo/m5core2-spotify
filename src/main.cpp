#include <Arduino.h>
#include <M5Unified.h>
#include <SpotifyEsp32.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "DisplayManager.h"
#include "SpotifyClient.h"
#include "secrets.h"

// Globals
// SpotifyEsp32 handles WiFiClientSecure internally? No, constructor takes
// client_id/secret only? Let's check constructor signature again. Spotify(const
// char* client_id, const char * client_secret, const char* refresh_token, int
// max_num_retry = 3); It creates its own WiFiClientSecure _client inside!! (See
// header line 709). So we don't pass a client.

Spotify spotify(SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET,
                SPOTIFY_REFRESH_TOKEN);

SpotifyClient spotifyClient(spotify, SPOTIFY_REFRESH_TOKEN);
DisplayManager displayMsg;

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 3000; // 3 seconds

// State vars
// State vars
String g_Title, g_Artist, g_Album, g_ArtUrl, g_TrackId;
String g_LastTrackId = "";
bool g_IsPlaying = false;
bool g_IsLiked = false;
bool g_LikeCheckNeeded = false;
int g_Progress = 0;
int g_Duration = 0;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  displayMsg.begin();
  displayMsg.showLoading("Connecting to WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int dotCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    dotCount++;
    if (dotCount > 10) {
      displayMsg.showLoading("WiFi Connecting...");
      dotCount = 0;
    }
  }

  displayMsg.showLoading("WiFi Connected! Setup Spotify...");

  // SpotifyEsp32.begin() might seem to be for web server auth?
  // Header says "start the server and begin authentication".
  // If we have refresh token, we might not need to call begin() for
  // *interactive* auth? But maybe for initialization? See examples if possible.
  // Usually libraries with refresh token Just Work. However, verify if
  // getAccessToken needs to be called.

  // We can try to just run. If methods return 401, the library should refresh.
  // Let's call a benign method to "wake up" or check auth.

  displayMsg.showLoading("Ready.");
}

void handleTouch() {
  auto t = M5.Touch.getDetail();
  if (t.wasPressed()) {
    int x = t.x;
    int y = t.y;

    // Only handle touch within the screen area (exclude physical button area
    // below screen)
    // Only handle touch within the screen area (exclude physical button area
    // below screen)
    if (y > 180 && y < 240) { // Bottom area on screen
      if (x < 110) {
        spotifyClient.previous();
      } else if (x > 110 && x < 210) {
        // Play/Pause
        if (g_IsPlaying)
          spotifyClient.pause();
        else
          spotifyClient.play();

        g_IsPlaying = !g_IsPlaying;
        displayMsg.updatePlaybackState(g_IsPlaying, g_Progress, g_Duration);
        lastUpdate = millis();
      } else if (x > 210) {
        spotifyClient.next();
      }
    } else if (x < 180 && y < 180) {
      // Like Button Area (Entire Artwork 180x180)

      if (g_IsLiked) {
        if (spotifyClient.unlikeTrack(g_TrackId.c_str())) {
          g_IsLiked = false;
        }
      } else {
        if (spotifyClient.likeTrack(g_TrackId.c_str())) {
          g_IsLiked = true;
        }
      }
      displayMsg.updateControlState(false, "off", g_IsLiked);
    }
  }
}

// Physical buttons
void handlePhysicalButtons() {
  if (M5.BtnA.wasPressed()) {
    spotifyClient.previous();
  }
  if (M5.BtnB.wasPressed()) {
    if (g_IsPlaying)
      spotifyClient.pause();
    else
      spotifyClient.play();

    // Optimistic UI update
    g_IsPlaying = !g_IsPlaying;
    displayMsg.updatePlaybackState(g_IsPlaying, g_Progress, g_Duration);
    lastUpdate = millis();
  }
  if (M5.BtnC.wasPressed()) {
    spotifyClient.next();
  }
}

void loop() {
  M5.update();
  handleTouch();
  handlePhysicalButtons();

  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    lastUpdate = millis();

    // Fetch Data
    // Fetch Data
    int status = spotifyClient.getNowPlaying(g_Title, g_Artist, g_Album,
                                             g_ArtUrl, g_TrackId, g_IsPlaying,
                                             g_Progress, g_Duration);

    if (status == 200) {
      if (g_TrackId != g_LastTrackId) {
        // Track Changed
        g_LastTrackId = g_TrackId;
        g_LikeCheckNeeded = true;
        // Reset Like State visually until checked
        g_IsLiked = false;
      }

      // Perform Like Check if needed
      if (g_LikeCheckNeeded) {
        bool likedState = false;
        if (spotifyClient.getLikeState(g_TrackId.c_str(), likedState)) {
          g_IsLiked = likedState;
          g_LikeCheckNeeded = false; // Check complete
        } else {
          // API Failed (e.g. rate limit, network).
          // Keep g_LikeCheckNeeded = true to retry next loop.
        }
      }

      displayMsg.updateNowPlaying(g_Title, g_Artist, g_Album, g_ArtUrl);
      displayMsg.updatePlaybackState(g_IsPlaying, g_Progress, g_Duration);
      displayMsg.updateControlState(false, "off",
                                    g_IsLiked); // Ensure button redraw
    } else {
      // Debug
      Serial.printf("Status: %d\n", status);
    }
  }
}
