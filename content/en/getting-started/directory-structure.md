---
title: Directory Structure
linktitle: Directory Structure
description: Hugo's CLI scaffolds a project directory structure and then takes that single directory and uses it as the input to create a complete website.
date: 2017-01-02
publishdate: 2017-02-01
lastmod: 2017-03-09
categories: [getting started,fundamentals]
keywords: [source, organization, directories]
menu:
  docs:
    parent: "getting-started"
    weight: 50
weight: 50
sections_weight: 50
draft: false
aliases: [/overview/source-directory/]
toc: true
---

## New Site Scaffolding

{{< youtube sB0HLHjgQ7E >}}

Running the `hugo new site` generator from the command line will create a directory structure with the following elements:

```
.
├── archetypes
├── config.toml
├── content
├── data
├── layouts
├── static
└── themes
```

