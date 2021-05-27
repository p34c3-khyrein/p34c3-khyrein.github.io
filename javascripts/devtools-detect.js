(function () {
  "use strict";

  const devtools = {
    isOpen: false,
    orientation: undefined,
  };

  const threshold = 160;

  const emitEvent = (isOpen, orientation) => {
    window.dispatchEvent(
      new CustomEvent("devtoolschange", {
        detail: {
          isOpen,
          orientation,
        },
      })
    );
  };

  const main = ({ emitEvents = true } = {}) => {
    const widthThreshold = window.outerWidth - window.innerWidth > threshold;
    const heightThreshold = window.outerHeight - window.innerHeight > threshold;
    const orientation = widthThreshold ? "vertical" : "horizontal";

    if (
      !(heightThreshold && widthThreshold) &&
      ((window.Firebug &&
        window.Firebug.chrome &&
        window.Firebug.chrome.isInitialized) ||
        widthThreshold ||
        heightThreshold)
    ) {
      if (
        (!devtools.isOpen || devtools.orientation !== orientation) &&
        emitEvents
      ) {
        emitEvent(true, orientation);
      }

      devtools.isOpen = true;
      devtools.orientation = orientation;
    } else {
      if (devtools.isOpen && emitEvents) {
        emitEvent(false, undefined);
      }

      devtools.isOpen = false;
      devtools.orientation = undefined;
    }
  };

  main({ emitEvents: false });
  setInterval(main, 500);

  if (typeof module !== "undefined" && module.exports) {
    module.exports = devtools;
  } else {
    window.devtools = devtools;
  }
})();

// Opera 8.0+
var isOpera =
  (!!window.opr && !!opr.addons) ||
  !!window.opera ||
  navigator.userAgent.indexOf(" OPR/") >= 0;

// Firefox 1.0+
var isFirefox = typeof InstallTrigger !== "undefined";

// Safari 3.0+ "[object HTMLElementConstructor]"
var isSafari =
  /constructor/i.test(window.HTMLElement) ||
  (function (p) {
    return p.toString() === "[object SafariRemoteNotification]";
  })(
    !window["safari"] ||
      (typeof safari !== "undefined" && safari.pushNotification)
  );

// Internet Explorer 6-11
var isIE = /*@cc_on!@*/ false || !!document.documentMode;

// Edge 20+
var isEdge = !isIE && !!window.StyleMedia;

// Chrome 1 - 79
var isChrome =
  !!window.chrome && (!!window.chrome.webstore || !!window.chrome.runtime);

// Edge (based on chromium) detection
var isEdgeChromium = isChrome && navigator.userAgent.indexOf("Edg") != -1;

// Blink engine detection
var isBlink = (isChrome || isOpera) && !!window.CSS;

function getBrowser() {
  if (isOpera) {
    return "Opera";
  } else if (isFirefox) {
    return "Firefox";
  } else if (isSafari) {
    return "Safari";
  } else if (isIE) {
    return "Internet Explorer";
  } else if (isEdge) {
    return "Microsoft Edge";
  } else if (isChrome) {
    return "Chrome";
  } else if (isEdgeChromium) {
    return "Microsoft Edge (based on chromium)";
  } else if (isBlink) {
    return "Blink";
  } else {
    return '';
  }
}

var value_setelah_inspek = `
lho koq ilang :V <br>
Browser ${getBrowser()} elu nggak mempan inspek gua :V <br>
Source bY <a href="https://p34c3-khyrein.linuxploit.com">P34C3_KHYREIN</a>

`;

//anti inspect element
if (window.devtools.isOpen) {
  try {
    console.log("mampuzzz :V");
    document.querySelector("html").remove();
    document.write(value_setelah_inspek);
  } catch (err) {
    //kosong
  }
}
window.addEventListener("devtoolschange", (event) => {
  try {
    console.log("mampuzzz :V");
    document.querySelector("html").remove();
    document.write(value_setelah_inspek);
  } catch (err) {
    //kosong
  }
});
