---
title: 'Branding dashboard'
description: 'Change dashboard colors and logo to brand it for your presentation'
date: '2021-05-13'
categories: [tutorials, dashboard]
keywords: [dashboard, branding, logo, colors]
weight: 12
---

In the plgd hub, customization of the UI for your company is possible. This includes the ability to modify the logo, colors, and even switch to your own preferred theme.

## Theme

To create your own theme, follow these steps:

1. Clone the repository by running the following command:

    ```sh
    git clone --recursive --branch v2.15.0 https://github.com/plgd-dev/hub.git
    ```

2. Navigate to the cloned directory and install the necessary packages:

    ```sh
    cd hub/http-gateway/web
    npm install
    ```

3. Create your own theme within the `./packages/shared-ui/src/components/Atomic/_theme/mycompany.ts` directory. You can refer to `./packages/shared-ui/src/components/Atomic/_theme/plgd.ts` as an example.

   ```sh
   cp ./packages/shared-ui/src/components/Atomic/_theme/plgd.ts ./packages/shared-ui/src/components/Atomic/_theme/mycompany.ts
   ```

4. Build your theme by executing the following command:

    ```sh
    node ./packages/shared-ui/scripts/build.theme.js --themes=mycompany
    ```

The generated theme will be stored in `./public/theme/theme.json`.

To implement these changes and refresh the UI, you have two options. First, you can set the theme in JSON format using `.Values.httpgateway.ui.theme` in the Helm values file. Alternatively, you can directly replace the file in the Docker image at the path `/usr/local/var/www/theme/theme.json`.

### Favicon

[`favicon.png`](https://github.com/plgd-dev/hub/tree/main/http-gateway/web/public/favicon.png)

You might need to adjust some CSS in order to have the Logo rendered correctly, if the size is different than the recommended one. You can modify these values in [`left-panel.scss`](https://github.com/plgd-dev/hub/tree/main/http-gateway/web/src/components/left-panel/left-panel.scss), look for the classes `.logo-big` and `.logo-small`. Adjust the height in these classes to fit your needs.

## Application name

The application name which also appears in the title bar can be changed by modifying the `appName` field in [`config.js`](https://github.com/plgd-dev/hub/tree/main/http-gateway/web/src/config.js).

## Header / Status Bar

The header has a dedicated component which can be found in [`status-bar.js`](https://github.com/plgd-dev/hub/tree/main/http-gateway/web/src/components/status-bar/status-bar.js). You can modify the status-bar `<header id="status-bar">...</header>` by removing existing components like `LanguageSwitcher` and `UserWidget` or by adding different content in the `header`.

## Footer

Footer has a dedicated component which can be found in [`footer.js`](https://github.com/plgd-dev/hub/tree/main/http-gateway/web/src/components/footer/footer.js). You can modify the `footer` tag by removing the already present links, or simply adding different content next to them.

## Text changes

Every text in this application is coming from a translation file located in [`languages.json`](https://github.com/plgd-dev/hub/blob/main/http-gateway/web/src/languages/languages.json). This object contains a language block for each language you support in your application. If a block is missing you can duplicate an existing block and modify the block with the language code that is missing.

Some messages might be missing. This is due to fact that they were not yet translated. You can add them manually or use a language editor like [POEditor](https://poeditor.com/).

You can also override these strings to fit your need, for example, if you would like to have `Lights` as the name of the menu which we call `Devices`, you have to find the key `menu.devices` and change its value to `Lights`.
