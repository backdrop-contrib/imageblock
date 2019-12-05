(function ($) {
  $(document).ready(function() {
    // Prevent ugly bg on huge screens.
    $('.imageblock-parallaxe').each(function() {
      var maxwidth = 2000;
      if ($('body > .layout').length) {
        maxwidth = $('body > .layout').width();
      }
      $(this).css('background-size', 'auto ' + maxwidth + 'px');
    });
    $(window).resize(function() {
      $('.imageblock-parallaxe').each(function() {
        var maxwidth = 2000;
        if ($('body > .layout').length) {
          maxwidth = $('body > .layout').width();
        }
        $(this).css('background-size', 'auto ' + maxwidth + 'px');
      });
    });
  });
})(jQuery);
