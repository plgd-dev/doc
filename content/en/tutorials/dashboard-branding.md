---
title: 'Branding dashboard'
description: 'Change dashboard colors and logo to brand it for your presentation'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [tutorials, dashboard]
keywords: [dashboard, branding, logo, colors]
menu:
  docs:
    parent: tutorials
    weight: 30
toc: true
---

Dashboard helps you to present to your stakeholders new device features as well as explain how this IoT framework works. To make it more convincing, basic templating functionality was introduced.

## Colors change

All colors are defined in one [`colors.scss`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/common/styles/colors.scss) file. Changing one of the colors will have an impact on all parts of the application.

## Logo change

You can change the logo of the application by replacing these files:

### Big logo (when the menu is expanded)

[`logo-big.svg`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/assets/img/logo-big.svg) - _Recommended size is 127px \* 35px_

### Small logo (when the menu is collapsed)

[`logo-small.svg`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/assets/img/logo-small.svg) - _recommended size is 45px \* 35px_

### Favicon

[`favicon.png`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/public/favicon.png)

You might need to adjust some CSS in order to have the Logo rendered correctly, if the size is different than the recommended one. You can modify these values in [`left-panel.scss`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/components/left-panel/left-panel.scss), look for the classes `.logo-big` and `.logo-small`. Adjust the height in these classes to fit your needs.

## Application name

The application name which also appears in the title bar can be changed by modifying the `appName` field in [`config.js`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/config.js).

## Header / Status Bar

The header has a dedicated component which can be found in [`status-bar.js`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/components/status-bar/status-bar.js). You can modify the status-bar `<header id="status-bar">...</header>` by removing existing components like `LanguageSwitcher` and `UserWidget` or by adding different content in the `header`.

## Footer

Footer has a dedicated component which can be found in [`footer.js`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/components/footer/footer.js). You can modify the `footer` tag by removing the already present links, or simply adding different content next to them.

## Text changes

Every text in this application is coming from a translation file located in [`langauges.json`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/languages/langauges.json). This object contains a language block for each language you support in your application. If a block is missing you can duplicate an existing block and modify the block with the language code that is missing.

Some messages might be missing. This is due to fact that they were not yet translated. You can add them manually or use a language editor like [POEditor](https://poeditor.com/).

You can also override these strings to fit your need, for example, if you would like to have `Devices` as the name of the menu which we call `Things`, you have to find the key `menu.things` and change its value to `Devices`.
