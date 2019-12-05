(function ($) {
  $(document).ready(function() {
    // Prevent ugly bg on huge screens.
    $('.imageblock-parallaxe').each(function() {
      let maxwidth = $(this).width();
      $(this).css('background-size', 'auto ' + maxwidth + 'px');
    });
    $(window).resize(function() {
      $('.imageblock-parallaxe').each(function() {
        let maxwidth = $(this).width();
        $(this).css('background-size', 'auto ' + maxwidth + 'px');
      });
    });
  });
})(jQuery);
