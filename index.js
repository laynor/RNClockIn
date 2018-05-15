import { app } from "./lib/js/re/app.js";
// import React from "react";
// import { AppRegistry } from 'react-native';

// AppRegistry.registerComponent('ClockIn', () => app);
/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  View
} from 'react-native';
import {Navigation} from 'react-native-navigation';

Navigation.registerComponent('app', () => app);
Navigation.startSingleScreenApp({
  screen: {
    screen: 'app',
    title: '🕟 Clock in'
  }
});
