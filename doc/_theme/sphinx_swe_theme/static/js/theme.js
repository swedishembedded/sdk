var jQuery = (typeof(window) != 'undefined') ? window.jQuery : require('jquery');

// Sphinx theme nav state
function ThemeNav () {

    var nav = {
        isRunning: false
    };

    nav.enable = function (withStickyNav) {
        var self = this;

        // TODO this can likely be removed once the theme javascript is broken
        // out from the RTD assets. This just ensures old projects that are
        // calling `enable()` get the sticky menu on by default. All other cals
        // to `enable` should include an argument for enabling the sticky menu.
        if (typeof(withStickyNav) == 'undefined') {
            withStickyNav = true;
        }

        if (self.isRunning) {
            // Only allow enabling nav logic once
            return;
        }

        self.isRunning = true;
        jQuery(function ($) {
            self.init($);
        });

    };

    nav.init = function ($) {
        var self = this;

        // Set up javascript UX bits
        $(document)
            // Shift nav in mobile when clicking the menu.
            .on('click', "[data-toggle='wy-nav-top']", function() {
                $("[data-toggle='wy-nav-shift']").toggleClass("shift");
            })

            // Nav menu link click operations
            .on('click', ".wy-menu-vertical .current ul li a", function() {
                var target = $(this);
                // Close menu when you click a link.
                $("[data-toggle='wy-nav-shift']").removeClass("shift");
                // Handle dynamic display of l3 and l4 nav lists
                self.toggleCurrent(target);
            })

        // Make tables responsive
        $("table.docutils:not(.field-list,.footnote,.citation)")
            .wrap("<div class='wy-table-responsive'></div>");

        // Add extra class to responsive tables that contain
        // footnotes or citations so that we can target them for styling
        $("table.docutils.footnote")
            .wrap("<div class='wy-table-responsive footnote'></div>");
        $("table.docutils.citation")
            .wrap("<div class='wy-table-responsive citation'></div>");

        // Add expand links to all parents of nested ul
        $('.wy-menu-vertical ul').not('.simple').siblings('a').each(function () {
            var link = $(this);
                expand =
                    $('<button class="toctree-expand" title="Open/close menu"></button>');
            expand.on('click', function (ev) {
                self.toggleCurrent(link);
                ev.stopPropagation();
                return false;
            });
            link.prepend(expand);
        });
    };

    nav.toggleCurrent = function (elem) {
        var parent_li = elem.closest('li');
        parent_li
            .siblings('li.current')
            .removeClass('current')
            .attr('aria-expanded','false');
        parent_li
            .siblings()
            .find('li.current')
            .removeClass('current')
            .attr('aria-expanded','false');
        var children = parent_li.find('> ul li');
        // Don't toggle terminal elements.
        if (children.length) {
            children
                .removeClass('current')
                .attr('aria-expanded','false');
            parent_li
                .toggleClass('current')
                .attr('aria-expanded', function(i, old) {
                    return old == 'true' ? 'false' : 'true';
                });
        }
    }

    return nav;
};

if (typeof(window) != 'undefined') {
    window.SphinxRtdTheme = {
        Navigation: ThemeNav(),
    };
}
