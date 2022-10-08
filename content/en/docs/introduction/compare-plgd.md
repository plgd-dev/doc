---
title: 'Compare plgd'
description: 'How plgd differentiates to other IoT platforms?'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [comparison, fundamentals]
keywords: [comparison, introduction]
toc: true
weight: 20
---

It’s tricky to keep track, with the number of IoT platforms out there. Which is dependable? Which is secure? Which fits my use case? Let’s see how they stack up.

{{< rawhtml >}}
<style>
    .comparison {
      border-spacing: 0;
      border-collapse: collapse;
    }

    #table th {
      padding: 0;
      border: 0;
      height: 50px;
      vertical-align: bottom;
      background-color: transparent;
    }
    .comparison th div {
      display: flex;
      padding: 6px 20px;
      width: 95px;
      font-size: 10pt;
      box-sizing: border-box;
      text-align: center;
      white-space: nowrap;
      background-color: #ccc;
      border-top-left-radius: 10px;
      border-top-right-radius: 10px;
      color: #000;
      justify-content: center;
      align-items: flex-end;
      transition-delay: 0s;
      transition-duration: 0.10s;
      transition-timing-function: linear;
      height: 50px;
    }
    .comparison th div.main {
      background-color: #FEBF40;
      height: 65px;
      vertical-align: bottom;
    }

    .comparison td {
      height: 35px;
      box-sizing: border-box;
      border: 1px solid #ccc;
      padding: 3px 8px;
      white-space: nowrap;
      transition-delay: 0s;
      transition-duration: 0.10s;
      transition-timing-function: linear;
    }
    .comparison td.tab {
      padding-left: 25px;
    }
    .comparison td:first-child {
      padding-right: 25px;
    }
    .comparison td:nth-child(2){
      background-color: #FEDA93 !important;
    }
    .comparison td:not(:first-child) {
      text-align: center;
    }

    .comparison td.hover:not(:first-child) {
      background-color: #f0f0f0;
    }

    .comparison th.hover div:not(.main) {
      height: 60px;
    }

    .comparison tbody:not(.empty) tr:nth-child(even) {background: #f7f7f7;}
    .comparison tbody:not(.empty) tr:nth-child(odd) {background: #FFF;}

    .hint {
    }

    /* Tooltip */
    .tooltip {
      background: #18191F;
      color: white;
      font-weight: bold;
      padding: 15px;
      font-size: 14px;
      font-weight: 400;
      line-height: 140%;
      box-sizing: border-box;
      border-radius: 4px;
      width: 300px;
      display: none;
      text-align: left;
      z-index: 5;
      word-wrap: break-word;
    }
    .tooltip span {
      display: block;
      white-space: normal;
    }
    .tooltip span strong {
      color: #FEBF40;
      font-size: 16px;
      margin-bottom: 4px;
      display: block;
    }
    .tooltip[data-show] {
      display: block;
    }
    .tooltip-arrow,
    .tooltip-arrow::before {
      position: absolute;
      width: 8px;
      height: 8px;
      background: inherit;
    }
    .tooltip-arrow {
      visibility: hidden;
    }
    .tooltip-arrow::before {
      visibility: visible;
      content: '';
      transform: rotate(45deg);
    }
    .tooltip[data-popper-placement^='top'] > .tooltip-arrow {
      bottom: -4px;
    }
    .tooltip[data-popper-placement^='bottom'] > .tooltip-arrow {
      top: -4px;
    }
    .tooltip[data-popper-placement^='left'] > .tooltip-arrow {
      right: -4px;
    }

    .tooltip[data-popper-placement^='right'] > .tooltip-arrow {
      left: -4px;
    }
  </style>
  <script type="text/javascript">
  /**
 * @popperjs/core v2.11.5 - MIT License
 */

!function(e,t){"object"==typeof exports&&"undefined"!=typeof module?t(exports):"function"==typeof define&&define.amd?define(["exports"],t):t((e="undefined"!=typeof globalThis?globalThis:e||self).Popper={})}(this,(function(e){"use strict";function t(e){if(null==e)return window;if("[object Window]"!==e.toString()){var t=e.ownerDocument;return t&&t.defaultView||window}return e}function n(e){return e instanceof t(e).Element||e instanceof Element}function r(e){return e instanceof t(e).HTMLElement||e instanceof HTMLElement}function o(e){return"undefined"!=typeof ShadowRoot&&(e instanceof t(e).ShadowRoot||e instanceof ShadowRoot)}var i=Math.max,a=Math.min,s=Math.round;function f(e,t){void 0===t&&(t=!1);var n=e.getBoundingClientRect(),o=1,i=1;if(r(e)&&t){var a=e.offsetHeight,f=e.offsetWidth;f>0&&(o=s(n.width)/f||1),a>0&&(i=s(n.height)/a||1)}return{width:n.width/o,height:n.height/i,top:n.top/i,right:n.right/o,bottom:n.bottom/i,left:n.left/o,x:n.left/o,y:n.top/i}}function c(e){var n=t(e);return{scrollLeft:n.pageXOffset,scrollTop:n.pageYOffset}}function p(e){return e?(e.nodeName||"").toLowerCase():null}function u(e){return((n(e)?e.ownerDocument:e.document)||window.document).documentElement}function l(e){return f(u(e)).left+c(e).scrollLeft}function d(e){return t(e).getComputedStyle(e)}function h(e){var t=d(e),n=t.overflow,r=t.overflowX,o=t.overflowY;return/auto|scroll|overlay|hidden/.test(n+o+r)}function m(e,n,o){void 0===o&&(o=!1);var i,a,d=r(n),m=r(n)&&function(e){var t=e.getBoundingClientRect(),n=s(t.width)/e.offsetWidth||1,r=s(t.height)/e.offsetHeight||1;return 1!==n||1!==r}(n),v=u(n),g=f(e,m),y={scrollLeft:0,scrollTop:0},b={x:0,y:0};return(d||!d&&!o)&&(("body"!==p(n)||h(v))&&(y=(i=n)!==t(i)&&r(i)?{scrollLeft:(a=i).scrollLeft,scrollTop:a.scrollTop}:c(i)),r(n)?((b=f(n,!0)).x+=n.clientLeft,b.y+=n.clientTop):v&&(b.x=l(v))),{x:g.left+y.scrollLeft-b.x,y:g.top+y.scrollTop-b.y,width:g.width,height:g.height}}function v(e){var t=f(e),n=e.offsetWidth,r=e.offsetHeight;return Math.abs(t.width-n)<=1&&(n=t.width),Math.abs(t.height-r)<=1&&(r=t.height),{x:e.offsetLeft,y:e.offsetTop,width:n,height:r}}function g(e){return"html"===p(e)?e:e.assignedSlot||e.parentNode||(o(e)?e.host:null)||u(e)}function y(e){return["html","body","#document"].indexOf(p(e))>=0?e.ownerDocument.body:r(e)&&h(e)?e:y(g(e))}function b(e,n){var r;void 0===n&&(n=[]);var o=y(e),i=o===(null==(r=e.ownerDocument)?void 0:r.body),a=t(o),s=i?[a].concat(a.visualViewport||[],h(o)?o:[]):o,f=n.concat(s);return i?f:f.concat(b(g(s)))}function x(e){return["table","td","th"].indexOf(p(e))>=0}function w(e){return r(e)&&"fixed"!==d(e).position?e.offsetParent:null}function O(e){for(var n=t(e),i=w(e);i&&x(i)&&"static"===d(i).position;)i=w(i);return i&&("html"===p(i)||"body"===p(i)&&"static"===d(i).position)?n:i||function(e){var t=-1!==navigator.userAgent.toLowerCase().indexOf("firefox");if(-1!==navigator.userAgent.indexOf("Trident")&&r(e)&&"fixed"===d(e).position)return null;var n=g(e);for(o(n)&&(n=n.host);r(n)&&["html","body"].indexOf(p(n))<0;){var i=d(n);if("none"!==i.transform||"none"!==i.perspective||"paint"===i.contain||-1!==["transform","perspective"].indexOf(i.willChange)||t&&"filter"===i.willChange||t&&i.filter&&"none"!==i.filter)return n;n=n.parentNode}return null}(e)||n}var j="top",E="bottom",D="right",A="left",L="auto",P=[j,E,D,A],M="start",k="end",W="viewport",B="popper",H=P.reduce((function(e,t){return e.concat([t+"-"+M,t+"-"+k])}),[]),T=[].concat(P,[L]).reduce((function(e,t){return e.concat([t,t+"-"+M,t+"-"+k])}),[]),R=["beforeRead","read","afterRead","beforeMain","main","afterMain","beforeWrite","write","afterWrite"];function S(e){var t=new Map,n=new Set,r=[];function o(e){n.add(e.name),[].concat(e.requires||[],e.requiresIfExists||[]).forEach((function(e){if(!n.has(e)){var r=t.get(e);r&&o(r)}})),r.push(e)}return e.forEach((function(e){t.set(e.name,e)})),e.forEach((function(e){n.has(e.name)||o(e)})),r}function C(e){return e.split("-")[0]}function q(e,t){var n=t.getRootNode&&t.getRootNode();if(e.contains(t))return!0;if(n&&o(n)){var r=t;do{if(r&&e.isSameNode(r))return!0;r=r.parentNode||r.host}while(r)}return!1}function V(e){return Object.assign({},e,{left:e.x,top:e.y,right:e.x+e.width,bottom:e.y+e.height})}function N(e,r){return r===W?V(function(e){var n=t(e),r=u(e),o=n.visualViewport,i=r.clientWidth,a=r.clientHeight,s=0,f=0;return o&&(i=o.width,a=o.height,/^((?!chrome|android).)*safari/i.test(navigator.userAgent)||(s=o.offsetLeft,f=o.offsetTop)),{width:i,height:a,x:s+l(e),y:f}}(e)):n(r)?function(e){var t=f(e);return t.top=t.top+e.clientTop,t.left=t.left+e.clientLeft,t.bottom=t.top+e.clientHeight,t.right=t.left+e.clientWidth,t.width=e.clientWidth,t.height=e.clientHeight,t.x=t.left,t.y=t.top,t}(r):V(function(e){var t,n=u(e),r=c(e),o=null==(t=e.ownerDocument)?void 0:t.body,a=i(n.scrollWidth,n.clientWidth,o?o.scrollWidth:0,o?o.clientWidth:0),s=i(n.scrollHeight,n.clientHeight,o?o.scrollHeight:0,o?o.clientHeight:0),f=-r.scrollLeft+l(e),p=-r.scrollTop;return"rtl"===d(o||n).direction&&(f+=i(n.clientWidth,o?o.clientWidth:0)-a),{width:a,height:s,x:f,y:p}}(u(e)))}function I(e,t,o){var s="clippingParents"===t?function(e){var t=b(g(e)),o=["absolute","fixed"].indexOf(d(e).position)>=0&&r(e)?O(e):e;return n(o)?t.filter((function(e){return n(e)&&q(e,o)&&"body"!==p(e)})):[]}(e):[].concat(t),f=[].concat(s,[o]),c=f[0],u=f.reduce((function(t,n){var r=N(e,n);return t.top=i(r.top,t.top),t.right=a(r.right,t.right),t.bottom=a(r.bottom,t.bottom),t.left=i(r.left,t.left),t}),N(e,c));return u.width=u.right-u.left,u.height=u.bottom-u.top,u.x=u.left,u.y=u.top,u}function _(e){return e.split("-")[1]}function F(e){return["top","bottom"].indexOf(e)>=0?"x":"y"}function U(e){var t,n=e.reference,r=e.element,o=e.placement,i=o?C(o):null,a=o?_(o):null,s=n.x+n.width/2-r.width/2,f=n.y+n.height/2-r.height/2;switch(i){case j:t={x:s,y:n.y-r.height};break;case E:t={x:s,y:n.y+n.height};break;case D:t={x:n.x+n.width,y:f};break;case A:t={x:n.x-r.width,y:f};break;default:t={x:n.x,y:n.y}}var c=i?F(i):null;if(null!=c){var p="y"===c?"height":"width";switch(a){case M:t[c]=t[c]-(n[p]/2-r[p]/2);break;case k:t[c]=t[c]+(n[p]/2-r[p]/2)}}return t}function z(e){return Object.assign({},{top:0,right:0,bottom:0,left:0},e)}function X(e,t){return t.reduce((function(t,n){return t[n]=e,t}),{})}function Y(e,t){void 0===t&&(t={});var r=t,o=r.placement,i=void 0===o?e.placement:o,a=r.boundary,s=void 0===a?"clippingParents":a,c=r.rootBoundary,p=void 0===c?W:c,l=r.elementContext,d=void 0===l?B:l,h=r.altBoundary,m=void 0!==h&&h,v=r.padding,g=void 0===v?0:v,y=z("number"!=typeof g?g:X(g,P)),b=d===B?"reference":B,x=e.rects.popper,w=e.elements[m?b:d],O=I(n(w)?w:w.contextElement||u(e.elements.popper),s,p),A=f(e.elements.reference),L=U({reference:A,element:x,strategy:"absolute",placement:i}),M=V(Object.assign({},x,L)),k=d===B?M:A,H={top:O.top-k.top+y.top,bottom:k.bottom-O.bottom+y.bottom,left:O.left-k.left+y.left,right:k.right-O.right+y.right},T=e.modifiersData.offset;if(d===B&&T){var R=T[i];Object.keys(H).forEach((function(e){var t=[D,E].indexOf(e)>=0?1:-1,n=[j,E].indexOf(e)>=0?"y":"x";H[e]+=R[n]*t}))}return H}var G={placement:"bottom",modifiers:[],strategy:"absolute"};function J(){for(var e=arguments.length,t=new Array(e),n=0;n<e;n++)t[n]=arguments[n];return!t.some((function(e){return!(e&&"function"==typeof e.getBoundingClientRect)}))}function K(e){void 0===e&&(e={});var t=e,r=t.defaultModifiers,o=void 0===r?[]:r,i=t.defaultOptions,a=void 0===i?G:i;return function(e,t,r){void 0===r&&(r=a);var i,s,f={placement:"bottom",orderedModifiers:[],options:Object.assign({},G,a),modifiersData:{},elements:{reference:e,popper:t},attributes:{},styles:{}},c=[],p=!1,u={state:f,setOptions:function(r){var i="function"==typeof r?r(f.options):r;l(),f.options=Object.assign({},a,f.options,i),f.scrollParents={reference:n(e)?b(e):e.contextElement?b(e.contextElement):[],popper:b(t)};var s,p,d=function(e){var t=S(e);return R.reduce((function(e,n){return e.concat(t.filter((function(e){return e.phase===n})))}),[])}((s=[].concat(o,f.options.modifiers),p=s.reduce((function(e,t){var n=e[t.name];return e[t.name]=n?Object.assign({},n,t,{options:Object.assign({},n.options,t.options),data:Object.assign({},n.data,t.data)}):t,e}),{}),Object.keys(p).map((function(e){return p[e]}))));return f.orderedModifiers=d.filter((function(e){return e.enabled})),f.orderedModifiers.forEach((function(e){var t=e.name,n=e.options,r=void 0===n?{}:n,o=e.effect;if("function"==typeof o){var i=o({state:f,name:t,instance:u,options:r}),a=function(){};c.push(i||a)}})),u.update()},forceUpdate:function(){if(!p){var e=f.elements,t=e.reference,n=e.popper;if(J(t,n)){f.rects={reference:m(t,O(n),"fixed"===f.options.strategy),popper:v(n)},f.reset=!1,f.placement=f.options.placement,f.orderedModifiers.forEach((function(e){return f.modifiersData[e.name]=Object.assign({},e.data)}));for(var r=0;r<f.orderedModifiers.length;r++)if(!0!==f.reset){var o=f.orderedModifiers[r],i=o.fn,a=o.options,s=void 0===a?{}:a,c=o.name;"function"==typeof i&&(f=i({state:f,options:s,name:c,instance:u})||f)}else f.reset=!1,r=-1}}},update:(i=function(){return new Promise((function(e){u.forceUpdate(),e(f)}))},function(){return s||(s=new Promise((function(e){Promise.resolve().then((function(){s=void 0,e(i())}))}))),s}),destroy:function(){l(),p=!0}};if(!J(e,t))return u;function l(){c.forEach((function(e){return e()})),c=[]}return u.setOptions(r).then((function(e){!p&&r.onFirstUpdate&&r.onFirstUpdate(e)})),u}}var Q={passive:!0};var Z={name:"eventListeners",enabled:!0,phase:"write",fn:function(){},effect:function(e){var n=e.state,r=e.instance,o=e.options,i=o.scroll,a=void 0===i||i,s=o.resize,f=void 0===s||s,c=t(n.elements.popper),p=[].concat(n.scrollParents.reference,n.scrollParents.popper);return a&&p.forEach((function(e){e.addEventListener("scroll",r.update,Q)})),f&&c.addEventListener("resize",r.update,Q),function(){a&&p.forEach((function(e){e.removeEventListener("scroll",r.update,Q)})),f&&c.removeEventListener("resize",r.update,Q)}},data:{}};var $={name:"popperOffsets",enabled:!0,phase:"read",fn:function(e){var t=e.state,n=e.name;t.modifiersData[n]=U({reference:t.rects.reference,element:t.rects.popper,strategy:"absolute",placement:t.placement})},data:{}},ee={top:"auto",right:"auto",bottom:"auto",left:"auto"};function te(e){var n,r=e.popper,o=e.popperRect,i=e.placement,a=e.variation,f=e.offsets,c=e.position,p=e.gpuAcceleration,l=e.adaptive,h=e.roundOffsets,m=e.isFixed,v=f.x,g=void 0===v?0:v,y=f.y,b=void 0===y?0:y,x="function"==typeof h?h({x:g,y:b}):{x:g,y:b};g=x.x,b=x.y;var w=f.hasOwnProperty("x"),L=f.hasOwnProperty("y"),P=A,M=j,W=window;if(l){var B=O(r),H="clientHeight",T="clientWidth";if(B===t(r)&&"static"!==d(B=u(r)).position&&"absolute"===c&&(H="scrollHeight",T="scrollWidth"),B=B,i===j||(i===A||i===D)&&a===k)M=E,b-=(m&&B===W&&W.visualViewport?W.visualViewport.height:B[H])-o.height,b*=p?1:-1;if(i===A||(i===j||i===E)&&a===k)P=D,g-=(m&&B===W&&W.visualViewport?W.visualViewport.width:B[T])-o.width,g*=p?1:-1}var R,S=Object.assign({position:c},l&&ee),C=!0===h?function(e){var t=e.x,n=e.y,r=window.devicePixelRatio||1;return{x:s(t*r)/r||0,y:s(n*r)/r||0}}({x:g,y:b}):{x:g,y:b};return g=C.x,b=C.y,p?Object.assign({},S,((R={})[M]=L?"0":"",R[P]=w?"0":"",R.transform=(W.devicePixelRatio||1)<=1?"translate("+g+"px, "+b+"px)":"translate3d("+g+"px, "+b+"px, 0)",R)):Object.assign({},S,((n={})[M]=L?b+"px":"",n[P]=w?g+"px":"",n.transform="",n))}var ne={name:"computeStyles",enabled:!0,phase:"beforeWrite",fn:function(e){var t=e.state,n=e.options,r=n.gpuAcceleration,o=void 0===r||r,i=n.adaptive,a=void 0===i||i,s=n.roundOffsets,f=void 0===s||s,c={placement:C(t.placement),variation:_(t.placement),popper:t.elements.popper,popperRect:t.rects.popper,gpuAcceleration:o,isFixed:"fixed"===t.options.strategy};null!=t.modifiersData.popperOffsets&&(t.styles.popper=Object.assign({},t.styles.popper,te(Object.assign({},c,{offsets:t.modifiersData.popperOffsets,position:t.options.strategy,adaptive:a,roundOffsets:f})))),null!=t.modifiersData.arrow&&(t.styles.arrow=Object.assign({},t.styles.arrow,te(Object.assign({},c,{offsets:t.modifiersData.arrow,position:"absolute",adaptive:!1,roundOffsets:f})))),t.attributes.popper=Object.assign({},t.attributes.popper,{"data-popper-placement":t.placement})},data:{}};var re={name:"applyStyles",enabled:!0,phase:"write",fn:function(e){var t=e.state;Object.keys(t.elements).forEach((function(e){var n=t.styles[e]||{},o=t.attributes[e]||{},i=t.elements[e];r(i)&&p(i)&&(Object.assign(i.style,n),Object.keys(o).forEach((function(e){var t=o[e];!1===t?i.removeAttribute(e):i.setAttribute(e,!0===t?"":t)})))}))},effect:function(e){var t=e.state,n={popper:{position:t.options.strategy,left:"0",top:"0",margin:"0"},arrow:{position:"absolute"},reference:{}};return Object.assign(t.elements.popper.style,n.popper),t.styles=n,t.elements.arrow&&Object.assign(t.elements.arrow.style,n.arrow),function(){Object.keys(t.elements).forEach((function(e){var o=t.elements[e],i=t.attributes[e]||{},a=Object.keys(t.styles.hasOwnProperty(e)?t.styles[e]:n[e]).reduce((function(e,t){return e[t]="",e}),{});r(o)&&p(o)&&(Object.assign(o.style,a),Object.keys(i).forEach((function(e){o.removeAttribute(e)})))}))}},requires:["computeStyles"]};var oe={name:"offset",enabled:!0,phase:"main",requires:["popperOffsets"],fn:function(e){var t=e.state,n=e.options,r=e.name,o=n.offset,i=void 0===o?[0,0]:o,a=T.reduce((function(e,n){return e[n]=function(e,t,n){var r=C(e),o=[A,j].indexOf(r)>=0?-1:1,i="function"==typeof n?n(Object.assign({},t,{placement:e})):n,a=i[0],s=i[1];return a=a||0,s=(s||0)*o,[A,D].indexOf(r)>=0?{x:s,y:a}:{x:a,y:s}}(n,t.rects,i),e}),{}),s=a[t.placement],f=s.x,c=s.y;null!=t.modifiersData.popperOffsets&&(t.modifiersData.popperOffsets.x+=f,t.modifiersData.popperOffsets.y+=c),t.modifiersData[r]=a}},ie={left:"right",right:"left",bottom:"top",top:"bottom"};function ae(e){return e.replace(/left|right|bottom|top/g,(function(e){return ie[e]}))}var se={start:"end",end:"start"};function fe(e){return e.replace(/start|end/g,(function(e){return se[e]}))}function ce(e,t){void 0===t&&(t={});var n=t,r=n.placement,o=n.boundary,i=n.rootBoundary,a=n.padding,s=n.flipVariations,f=n.allowedAutoPlacements,c=void 0===f?T:f,p=_(r),u=p?s?H:H.filter((function(e){return _(e)===p})):P,l=u.filter((function(e){return c.indexOf(e)>=0}));0===l.length&&(l=u);var d=l.reduce((function(t,n){return t[n]=Y(e,{placement:n,boundary:o,rootBoundary:i,padding:a})[C(n)],t}),{});return Object.keys(d).sort((function(e,t){return d[e]-d[t]}))}var pe={name:"flip",enabled:!0,phase:"main",fn:function(e){var t=e.state,n=e.options,r=e.name;if(!t.modifiersData[r]._skip){for(var o=n.mainAxis,i=void 0===o||o,a=n.altAxis,s=void 0===a||a,f=n.fallbackPlacements,c=n.padding,p=n.boundary,u=n.rootBoundary,l=n.altBoundary,d=n.flipVariations,h=void 0===d||d,m=n.allowedAutoPlacements,v=t.options.placement,g=C(v),y=f||(g===v||!h?[ae(v)]:function(e){if(C(e)===L)return[];var t=ae(e);return[fe(e),t,fe(t)]}(v)),b=[v].concat(y).reduce((function(e,n){return e.concat(C(n)===L?ce(t,{placement:n,boundary:p,rootBoundary:u,padding:c,flipVariations:h,allowedAutoPlacements:m}):n)}),[]),x=t.rects.reference,w=t.rects.popper,O=new Map,P=!0,k=b[0],W=0;W<b.length;W++){var B=b[W],H=C(B),T=_(B)===M,R=[j,E].indexOf(H)>=0,S=R?"width":"height",q=Y(t,{placement:B,boundary:p,rootBoundary:u,altBoundary:l,padding:c}),V=R?T?D:A:T?E:j;x[S]>w[S]&&(V=ae(V));var N=ae(V),I=[];if(i&&I.push(q[H]<=0),s&&I.push(q[V]<=0,q[N]<=0),I.every((function(e){return e}))){k=B,P=!1;break}O.set(B,I)}if(P)for(var F=function(e){var t=b.find((function(t){var n=O.get(t);if(n)return n.slice(0,e).every((function(e){return e}))}));if(t)return k=t,"break"},U=h?3:1;U>0;U--){if("break"===F(U))break}t.placement!==k&&(t.modifiersData[r]._skip=!0,t.placement=k,t.reset=!0)}},requiresIfExists:["offset"],data:{_skip:!1}};function ue(e,t,n){return i(e,a(t,n))}var le={name:"preventOverflow",enabled:!0,phase:"main",fn:function(e){var t=e.state,n=e.options,r=e.name,o=n.mainAxis,s=void 0===o||o,f=n.altAxis,c=void 0!==f&&f,p=n.boundary,u=n.rootBoundary,l=n.altBoundary,d=n.padding,h=n.tether,m=void 0===h||h,g=n.tetherOffset,y=void 0===g?0:g,b=Y(t,{boundary:p,rootBoundary:u,padding:d,altBoundary:l}),x=C(t.placement),w=_(t.placement),L=!w,P=F(x),k="x"===P?"y":"x",W=t.modifiersData.popperOffsets,B=t.rects.reference,H=t.rects.popper,T="function"==typeof y?y(Object.assign({},t.rects,{placement:t.placement})):y,R="number"==typeof T?{mainAxis:T,altAxis:T}:Object.assign({mainAxis:0,altAxis:0},T),S=t.modifiersData.offset?t.modifiersData.offset[t.placement]:null,q={x:0,y:0};if(W){if(s){var V,N="y"===P?j:A,I="y"===P?E:D,U="y"===P?"height":"width",z=W[P],X=z+b[N],G=z-b[I],J=m?-H[U]/2:0,K=w===M?B[U]:H[U],Q=w===M?-H[U]:-B[U],Z=t.elements.arrow,$=m&&Z?v(Z):{width:0,height:0},ee=t.modifiersData["arrow#persistent"]?t.modifiersData["arrow#persistent"].padding:{top:0,right:0,bottom:0,left:0},te=ee[N],ne=ee[I],re=ue(0,B[U],$[U]),oe=L?B[U]/2-J-re-te-R.mainAxis:K-re-te-R.mainAxis,ie=L?-B[U]/2+J+re+ne+R.mainAxis:Q+re+ne+R.mainAxis,ae=t.elements.arrow&&O(t.elements.arrow),se=ae?"y"===P?ae.clientTop||0:ae.clientLeft||0:0,fe=null!=(V=null==S?void 0:S[P])?V:0,ce=z+ie-fe,pe=ue(m?a(X,z+oe-fe-se):X,z,m?i(G,ce):G);W[P]=pe,q[P]=pe-z}if(c){var le,de="x"===P?j:A,he="x"===P?E:D,me=W[k],ve="y"===k?"height":"width",ge=me+b[de],ye=me-b[he],be=-1!==[j,A].indexOf(x),xe=null!=(le=null==S?void 0:S[k])?le:0,we=be?ge:me-B[ve]-H[ve]-xe+R.altAxis,Oe=be?me+B[ve]+H[ve]-xe-R.altAxis:ye,je=m&&be?function(e,t,n){var r=ue(e,t,n);return r>n?n:r}(we,me,Oe):ue(m?we:ge,me,m?Oe:ye);W[k]=je,q[k]=je-me}t.modifiersData[r]=q}},requiresIfExists:["offset"]};var de={name:"arrow",enabled:!0,phase:"main",fn:function(e){var t,n=e.state,r=e.name,o=e.options,i=n.elements.arrow,a=n.modifiersData.popperOffsets,s=C(n.placement),f=F(s),c=[A,D].indexOf(s)>=0?"height":"width";if(i&&a){var p=function(e,t){return z("number"!=typeof(e="function"==typeof e?e(Object.assign({},t.rects,{placement:t.placement})):e)?e:X(e,P))}(o.padding,n),u=v(i),l="y"===f?j:A,d="y"===f?E:D,h=n.rects.reference[c]+n.rects.reference[f]-a[f]-n.rects.popper[c],m=a[f]-n.rects.reference[f],g=O(i),y=g?"y"===f?g.clientHeight||0:g.clientWidth||0:0,b=h/2-m/2,x=p[l],w=y-u[c]-p[d],L=y/2-u[c]/2+b,M=ue(x,L,w),k=f;n.modifiersData[r]=((t={})[k]=M,t.centerOffset=M-L,t)}},effect:function(e){var t=e.state,n=e.options.element,r=void 0===n?"[data-popper-arrow]":n;null!=r&&("string"!=typeof r||(r=t.elements.popper.querySelector(r)))&&q(t.elements.popper,r)&&(t.elements.arrow=r)},requires:["popperOffsets"],requiresIfExists:["preventOverflow"]};function he(e,t,n){return void 0===n&&(n={x:0,y:0}),{top:e.top-t.height-n.y,right:e.right-t.width+n.x,bottom:e.bottom-t.height+n.y,left:e.left-t.width-n.x}}function me(e){return[j,D,E,A].some((function(t){return e[t]>=0}))}var ve={name:"hide",enabled:!0,phase:"main",requiresIfExists:["preventOverflow"],fn:function(e){var t=e.state,n=e.name,r=t.rects.reference,o=t.rects.popper,i=t.modifiersData.preventOverflow,a=Y(t,{elementContext:"reference"}),s=Y(t,{altBoundary:!0}),f=he(a,r),c=he(s,o,i),p=me(f),u=me(c);t.modifiersData[n]={referenceClippingOffsets:f,popperEscapeOffsets:c,isReferenceHidden:p,hasPopperEscaped:u},t.attributes.popper=Object.assign({},t.attributes.popper,{"data-popper-reference-hidden":p,"data-popper-escaped":u})}},ge=K({defaultModifiers:[Z,$,ne,re]}),ye=[Z,$,ne,re,oe,pe,le,de,ve],be=K({defaultModifiers:ye});e.applyStyles=re,e.arrow=de,e.computeStyles=ne,e.createPopper=be,e.createPopperLite=ge,e.defaultModifiers=ye,e.detectOverflow=Y,e.eventListeners=Z,e.flip=pe,e.hide=ve,e.offset=oe,e.popperGenerator=K,e.popperOffsets=$,e.preventOverflow=le,Object.defineProperty(e,"__esModule",{value:!0})}));
  </script>
  <script type="text/javascript">
    document.addEventListener("DOMContentLoaded", function(event) {
      function setCellClasses(index) {
        document.querySelectorAll('#table td, #table th')
        .forEach(elem => {
          if (elem.cellIndex === index) {
            elem.classList.add("hover");
          }
        });
      }

      document.querySelectorAll('#table td, #table th')
      .forEach(e => e.addEventListener("mouseenter", function(event) {
        setCellClasses(event.target.cellIndex);
      }));

      document.querySelector("#table").addEventListener("mouseleave", function(event) {
        document.querySelectorAll("td, th").forEach(e => e.classList.remove("hover"));
      })

      document.querySelector("#table").addEventListener("mouseover", function(event) {
        document.querySelectorAll("td, th").forEach(e => e.classList.remove("hover"));
      });

      // Tooltips
      var showEvents = ["mouseenter", "focus"];
      var hideEvents = ["mouseleave", "blur"];

      // Tooltip automization
      var buttons = document.getElementsByClassName("hint");
      for (var i = 0; i < buttons.length; i++) {
        const button = buttons[i];
        const tooltip = document.querySelector("#" + button.dataset.tip);
        const popperInstance = Popper.createPopper(button, tooltip, {
          placement: 'auto',
          modifiers: [
            {
              name: "offset",
              options: {
                offset: [0, 8],
              },
            },
          ],
        });

        showEvents.forEach((event) => {
          button.addEventListener(event, () => {
            tooltip.setAttribute("data-show", "");
            button.classList.add("paused");
            popperInstance.update();
          });
        });

        hideEvents.forEach((event) => {
          button.addEventListener(event, () => {
            tooltip.removeAttribute("data-show");
            button.classList.remove("paused");
            popperInstance.update();
          });
        });
      }
    });
  </script>
  <div class="table-responsive">
  <table class="comparison" id="table">
    <thead>
      <tr>
        <th></th>
        <th><div class="main">plgd</div></th>
        <th><div>AWS<br />IoT Core</div></th>
        <th><div>Azure IoT</div></th>
        <th><div>Siemens<br />MindSphere</div></th>
        <th><div>Google<br />Cloud IoT</div></th>
        <th><div>Particle</div></th>
        <th><div>ThingWorkx</div></th>
        <th><div>OpenRemote</div></th>
        <th><div>ThingsBoard</div></th>
      </tr>
    </thead>

    <tbody>
      <tr>
        <td><strong>General</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
          <span class="hint tooltip-00" data-tip="tooltip-00" tabindex="1">ⓘ Open Source</span>
          <div id="tooltip-00" class="tooltip" role="tooltip">
            <span>Device library, IoT backend, and other tools are freely available on GitHub.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-01" data-tip="tooltip-01" tabindex="1">ⓘ ISO Ratified Application Protocol</span>
          <div id="tooltip-01" class="tooltip" role="tooltip">
            <span>This set of standards governs how applications communicate, ensuring interoperability between applications and platforms.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Communication models</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-02" data-tip="tooltip-02" tabindex="1">ⓘ Device to Device</span>
          <div id="tooltip-02" class="tooltip" role="tooltip">
            <span>Direct TCP/UDP communication between devices or applications and devices, without the need for a protocol gateway, bridge, or any additional system.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-03" data-tip="tooltip-03" tabindex="1">ⓘ Device discovery</span>
          <div id="tooltip-03" class="tooltip" role="tooltip">
            <span>CoAP multicast based discovery of devices, with filters for device types or their capabilities.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-04" data-tip="tooltip-04" tabindex="1">ⓘ Device to Cloud</span>
          <div id="tooltip-04" class="tooltip" role="tooltip">
            <span>Send data to the remote endpoint, the hosted backend system.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-05" data-tip="tooltip-05" tabindex="1">ⓘ Cloud to Device</span>
          <div id="tooltip-05" class="tooltip" role="tooltip">
            <span>Send commands from your applications to your remote devices, as part of bidirectional communication.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-06" data-tip="tooltip-06" tabindex="1">ⓘ Device to Device over Cloud</span>
          <div id="tooltip-06" class="tooltip" role="tooltip">
            <span>Ability of one device to communicate with another, through the backend system.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
        <td></td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Ecosystem</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-07" data-tip="tooltip-07" tabindex="1">ⓘ Device Management</span>
          <div id="tooltip-07" class="tooltip" role="tooltip">
            <span>Portal, to observe and manage your IoT fleet.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-08" data-tip="tooltip-08" tabindex="1">ⓘ Device Library</span>
          <div id="tooltip-08" class="tooltip" role="tooltip">
            <span>The platform natively supports the major MCUs and operating systems through its native device library.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-09" data-tip="tooltip-09" tabindex="1">ⓘ Device Twin / Shadow</span>
          <div id="tooltip-09" class="tooltip" role="tooltip">
            <span>Monitor the status of the device and its capabilities / sensors. Up to date and available even if the device is offline.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td></td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-10" data-tip="tooltip-10" tabindex="1">ⓘ Pending / Offline commands</span>
          <div id="tooltip-10" class="tooltip" role="tooltip">
            <span>Commands can be issued to a device when it is offline, and will execute when the device comes back online.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
        <td>X</td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Deployment</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-11" data-tip="tooltip-11" tabindex="1">ⓘ Off-premise</span>
          <div id="tooltip-11" class="tooltip" role="tooltip">
            <span>Hosted in the data center / cloud of your choice; managed by you.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-12" data-tip="tooltip-12" tabindex="1">ⓘ On-premise</span>
          <div id="tooltip-12" class="tooltip" role="tooltip">
            <span>Deployed in-house; managed by you.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-13" data-tip="tooltip-13" tabindex="1">ⓘ Managed</span>
          <div id="tooltip-13" class="tooltip" role="tooltip">
            <span>Hosted and operated by service providers.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td>X</td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Device provisioning</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-14" data-tip="tooltip-14" tabindex="1">ⓘ Manufacturer certificates</span>
          <div id="tooltip-14" class="tooltip" role="tooltip">
            <span>Verify device authenticity with manufacturer certificates.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-15" data-tip="tooltip-15" tabindex="1">ⓘ Random PIN</span>
          <div id="tooltip-15" class="tooltip" role="tooltip">
            <span>Provision devices for device-to-device communication with a random PIN displayed on the device.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-16" data-tip="tooltip-16" tabindex="1">ⓘ Preshared key</span>
          <div id="tooltip-16" class="tooltip" role="tooltip">
            <span>Method to authenticate a device by sharing a secret key between two devices.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-17" data-tip="tooltip-17" tabindex="1">ⓘ Identity Certificates</span>
          <div id="tooltip-17" class="tooltip" role="tooltip">
            <span>After authentication, a device can be provisioned with custom identity certificates. These certificates are used for encryption and device identification.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-18" data-tip="tooltip-18" tabindex="1">ⓘ Device ACLs</span>
          <div id="tooltip-18" class="tooltip" role="tooltip">
            <span>Specifications dictating actions allowed on certain resources. Applies to devices and/or clouds.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-19" data-tip="tooltip-19" tabindex="1">ⓘ OAuth 2.0</span>
          <div id="tooltip-19" class="tooltip" role="tooltip">
            <span>Employing OAuth2.0 server and JWT tokens for device to cloud as well as cloud to device communications.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Zero-touch provisioning</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-201" data-tip="tooltip-201" tabindex="1">ⓘ x509 attestation</span>
          <div id="tooltip-201" class="tooltip" role="tooltip">
            <span>Automated provisioning of the device after first startup, verifying its identity using its manufacturer certificate.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-202" data-tip="tooltip-202" tabindex="1">ⓘ Device pre-configuration</span>
          <div id="tooltip-202" class="tooltip" role="tooltip">
            <span>Ability to define initial configuration, which should be set to the device during automated provisioning.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-203" data-tip="tooltip-203" tabindex="1">ⓘ Certificate rotation</span>
          <div id="tooltip-203" class="tooltip" role="tooltip">
            <span>Rotation of identity certificates, set by end-customer, used to encrypt communication between devices and between devices and the cloud.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-204" data-tip="tooltip-204" tabindex="1">ⓘ Reprovisioning</span>
          <div id="tooltip-204" class="tooltip" role="tooltip">
            <span>Change your enrollment group configuration to allow it to automatically reconfigure, connect to a different cloud, change ACLs, rotate tokens, or renew identity certificates.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Interoperability</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-20" data-tip="tooltip-20" tabindex="1">ⓘ Standardized device models</span>
          <div id="tooltip-20" class="tooltip" role="tooltip">
            <span>Pre-defined models for various sensors and use cases.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-21" data-tip="tooltip-21" tabindex="1">ⓘ OpenAPI for Device</span>
          <div id="tooltip-21" class="tooltip" role="tooltip">
            <span>Easily define and document your own device models, with OpenAPI.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-22" data-tip="tooltip-22" tabindex="1">ⓘ Resource discovery</span>
          <div id="tooltip-22" class="tooltip" role="tooltip">
            <span>Send a multicast with the resource type filter / capability, and only devices with this capability will reply.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-23" data-tip="tooltip-23" tabindex="1">ⓘ Conformance testing</span>
          <div id="tooltip-23" class="tooltip" role="tooltip">
            <span>One instance of the system supports device management of multiple customers at the same time.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
    </tbody>

    <tbody>
      <tr>
        <td><strong>Security</strong></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        Mutual TLS
        </td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
        <td>X</td>
      </tr>
      <tr>
        <td class="tab">
        DTLS
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-26" data-tip="tooltip-26" tabindex="1">ⓘ End-to-end encryption</span>
          <div id="tooltip-26" class="tooltip" role="tooltip">
            <span>End-to-end encryption using OSCORE.</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td class="tab">
        <span class="hint tooltip-27" data-tip="tooltip-27" tabindex="1">ⓘ Single instance multitenancy</span>
          <div id="tooltip-27" class="tooltip" role="tooltip">
            <span>Testing and certification of conformance to Open Connectivity (OCF) ISO standards of interoperability (and OCF’s other quality standards).</span>
            <div class="tooltip-arrow" data-popper-arrow></div>
          </div>
        </td>
        <td>X</td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td>X</td>
      </tr>
    </tbody>
  </table>
  </div>
{{< /rawhtml >}}