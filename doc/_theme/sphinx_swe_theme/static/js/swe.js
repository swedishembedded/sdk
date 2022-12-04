function NCS () {
  "use strict";

  let state = {};

  // XXX: do not remove the trailing '/'
  const SWE_PATH_PREFIX = "/sdk/doc/";
  const STABLE_VERSION_RE = /^(\d+\.)+\d+$/;
  const DEV_VERSION_RE = /^(\d+\.)+\d+-[a-z0-9]+$/;
  const LOCALHOST_RE = /^(localhost)|((\d{1,3}\.){3}\d{1,3}):\d{4,5}/

  /*
   * Allow running from localhost; local build can be served with:
   * python -m http.server
   */
  state.updateLocations = function(){
    const host = window.location.host;
    let root_suffix = "";
    if (LOCALHOST_RE.test(host)) {
      this.url_prefix = "/";
    } else {
      this.url_prefix = SWE_PATH_PREFIX;
      root_suffix = "latest";
    }

    this.url_root = window.location.protocol + "//" + host + this.url_prefix;
    this.version_data_url = this.url_root + root_suffix + "/versions.json";
  };

  /*
   * Find the index of the first element that matches a X.Y.Z release, this
   * is considered the last "official" release.
   */
  state.findLastVersionIndex = function() {
    const versions = window.NCS.versions.VERSIONS;
    window.NCS.last_version_index = 1;
    for (var index in versions) {
      if (STABLE_VERSION_RE.test(versions[index])) {
        window.NCS.last_version_index = index;
        break;
      }
    }
  }

  /*
   * Infer the currently running version of the documentation
   */
  state.findCurrentVersion = function() {
    const path = window.location.pathname;
    if (path.startsWith(this.url_prefix)) {
      const prefix_len = this.url_prefix.length;
      window.NCS.current_version = path.slice(prefix_len).split("/")[0];
    } else {
      window.NCS.current_version = "latest";
    }
  };

  /*
   * Infer the current page being browsed, stripped from any fixed and
   * versioned prefix; it'll be used to jump to the same page in another
   * docset version.
   */
  state.findCurrentPage = function() {
    const path = window.location.pathname;
    const version_prefix = window.NCS.current_version + "/";
    if (path.startsWith(this.url_prefix)) {
      const prefix_len = this.url_prefix.length;
      let new_page = path.slice(prefix_len);
      if (new_page.startsWith(version_prefix)) {
        new_page = new_page.slice(version_prefix.length);
      }
      window.NCS.current_page = new_page;
    } else {
      window.NCS.current_page = "sdk/index.html";
    }
  };

  /*
   * Updates the dropbox of nRF Connect SDK versions displayed below the
   * current development version, and links to the same page currently being
   * browsed in those earlier releases.
   */
  state.updateVersionDropDown = function() {
    const ncs = window.NCS;
    const versions = ncs.versions.VERSIONS;
    const by_version = ncs.versions.COMPONENTS_BY_VERSION[ncs.current_version] ||
                       ncs.versions.COMPONENTS_BY_VERSION["latest"];

    // Update dropdown text
    const ncs_version = by_version["ncs"];
    $("#ncsversion").text(`v${ncs_version}`);

    // Update dropdown content
    $.each(versions, function(_, v) {
      if (v !== ncs.current_version) {
        let link = `<a class="dropdown-item" href=${ncs.url_root + v}/${ncs.current_page}>${v}</a>`;
        $("div.dropdown-menu").append(link);
      }
    });
  };

  /*
   * Display a message at the top of the page to inform the user that the
   * version currently being browsed is not the latest.
   */
  state.showVersion = function() {
    const ncs = window.NCS;
    const VERSIONS = ncs.versions.VERSIONS;
    const last_version_index = ncs.last_version_index;
    const path_suffix = "/" +  ncs.current_page;
    const last_release_url = ncs.url_root + VERSIONS[last_version_index] + path_suffix;
    const latest_release_url = ncs.url_root + "latest" + path_suffix;

    const SWITCH_MSG = "You might want to switch to the documentation for " +
      "the <a href='" + last_release_url + "'>" + VERSIONS[last_version_index] +
      "</a> release or the <a href='" + latest_release_url + "'>current " +
      "state of development</a>."

    const OLD_RELEASE_MSG =
      "You are looking at an older version of the documentation. " + SWITCH_MSG;

    const DEV_RELEASE_MSG =
      "You are looking at the documentation for a development tag. " + SWITCH_MSG;

    const LAST_RELEASE_MSG =
      "You are looking at the documentation for the latest official release.";

    if (ncs.current_version === VERSIONS[last_version_index]) {
      $("div.announcement").html(LAST_RELEASE_MSG);
      $("div.announcement").show();
    } else if (DEV_VERSION_RE.test(ncs.current_version)) {
      $("div.announcement").html(DEV_RELEASE_MSG);
      $("div.announcement").show();
    } else if (VERSIONS.includes(ncs.current_version) &&
               ncs.current_version !== VERSIONS[0]) {
      $("div.announcement").html(OLD_RELEASE_MSG);
      $("div.announcement").show();
    }
  };

  state.updatePage = function() {
    let ncs = window.NCS;
    ncs.findLastVersionIndex();
    ncs.findCurrentVersion();
    ncs.findCurrentPage();
    ncs.updateVersionDropDown();
    ncs.showVersion();
  };

  const NCS_SESSION_KEY = "ncs";

  /*
   * Load a versions.json from the session cache if available
   */
  state.loadVersions = function() {
    let versions_data = window.sessionStorage.getItem(NCS_SESSION_KEY);
    if (versions_data) {
      window.NCS.versions = JSON.parse(versions_data);
      return true;
    }
    return false;
  }

  /*
   * Update the session cache with a new versions.json
   */
  state.saveVersions = function(versions_data) {
    const session_value = JSON.stringify(versions_data);
    window.sessionStorage.setItem(NCS_SESSION_KEY, session_value);
    window.NCS.versions = versions_data;
  }

  /*
   * When the "Hide Search Matches" (from Sphinx's doctools) link is clicked,
   * rewrite the URL to remove the search term.
   */
  state.hideSearchMatches = function() {
    $('.highlight-link > a').on('click', function(){
      // Remove any ?highlight=* search querystring element
      window.location.assign(
        window.location.href.replace(/[?]highlight=[^#]*/, '')
      );
    });
  }

  return state;
};

if (typeof window !== 'undefined') {
  window.NCS = NCS();
}

$(document).ready(function(){
  window.NCS.updateLocations();
  window.NCS.hideSearchMatches();

  if (window.NCS.loadVersions()) {
    window.NCS.updatePage();
  } else {
    /* Get versions file from remote server. */
    $.getJSON(window.NCS.version_data_url, function(json_data) {
      window.NCS.saveVersions(json_data);
      window.NCS.updatePage();
    });
  }
});
