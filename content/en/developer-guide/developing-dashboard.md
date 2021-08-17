---
title: 'Dashboard'
description: 'How to build and configure plgd dashboard'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [developer guide, dashboard]
keywords: [developer guide, dashboard]
menu:
  docs:
    parent: developer-guide
    weight: 10
toc: true
---

Configuration for the client can be found in [`auth_config.json`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/auth_config.json).

```json
{
  "domain": "auth.plgd.cloud",
  "clientID": "pHdCKhnpgGEtU7KAPcLYCoCAkZ4cYVZg",
  "audience": "https://try.plgd.cloud",
  "scope": "",
  "httpGatewayAddress": "https://api.try.plgd.cloud"
}
```

## Installation and starting the development server

```bash
npm install
npm start
```

Application will be hosted on `http://localhost:3000` by default. To change the default port, put `PORT=xxxx` into `package.json` script for starting the development server

```bash
cross-env PORT=3000 craco start
```

or set `PORT` into your environment variables.

## Building the app

```bash
npm run build
```

## Translations

In order to add a new language, open the file [`config.js`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/config.js) and extend the `supportedLanguages` array with additional values. Use the [language code 2](https://www.science.co.il/language/Codes.php) for the language you are about to add:

`supportedLanguages: ['en', 'sk']`

Once you added a new language, open the file [`language-switcher-i18n.js`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/components/language-switcher/language-switcher-i18n.js) and add a new entry for the language you added. For example, for the language with code `sk` the entry would look like this:

```javascript
sk: {
  id: 'language-switcher.slovak',
  defaultMessage: 'Slovak',
},
```

### Generating language files

For extracting the messages from the UI components, run the following script:

```bash
npm run generate-pot
```

This script will generate a `template.pot` file, which contains all the strings from the application ready to be translated. Upload this file to your translation tool, translate the strings and after that, export the `.po` files for all the translations and place them into [`i18n`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/i18n) folder.

For generating language files which are used by the application, run the following script:

```bash
npm run generate-language-files
```

Now your translations are updated and ready to be used.

### Default language

The default language is set to be `en`. This configuration can be overridden in [`config.js`](https://github.com/plgd-dev/cloud/tree/v2/http-gateway/web/src/config.js) by changing the `defaultLanguage` field. Change this only to a language that is supported by the application (is present in the `supportedLanguages` list).

After your first visit to the application from a browser, the application will remember the current language state. In order to change the `defaultLanguage` field and see the change, you should clear the `localStorage` entry `language` from the browser.
