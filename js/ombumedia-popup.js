(function($) {
"use strict";

/**
 * Functionality inside the file selection pop-up.
 */



/**
 * Initializes the Library/Upload/Web tabs.
 */
Drupal.behaviors.ombumediaPopupTabs = {
  attach: function(context) {
    $('.ombumedia-tabset').tabs();
  }
};



/**
 * Binds the final file seleciton click.
 */
Drupal.behaviors.ombumediaSelectConfigure = {
  attach: function(context) {
    $('.select-link[data-fid]', context)
      .once('select-link-fid')
      .on('click', function(e) {
        e.preventDefault();
        var $link = $(this);
        var fid = $link.attr('data-fid');
        if (ombumediaSelectCallback) {
          ombumediaSelectCallback(fid, 'full');
        }
      });
  }
};



/**
 * Drag n Drop uploading in popup.
 */

var preventDefault = Drupal.ombumedia.util.preventDefaultWrapper;
var stopPropagation = Drupal.ombumedia.util.stopPropagationWrapper;
var fileUrl = Drupal.ombumedia.util.fileUrl;

Drupal.behaviors.ombumediaDragUpload = {
  attach: function(context) {
    var $body = $('body.page-admin-dashboard-select-media').once('drag-upload');

    if (!$body.length) {
      return;
    }

    var $dragOverlay = $('<div class="ombumedia-upload-drag"><span>Drop file to upload</span></div>').appendTo($body);
    var $uploadOverlay = $(['',
                            '<div class="ombumedia-upload-progress">',
                              '<div class="progress-bar">',
                                '<span class="bar"></span>',
                                '<span class="progress-text">Uploading <span class="progress-text-filename"></span></span>',
                              '</div>',
                            '</div>',
                          ''].join('')).appendTo($body);
    var $progressFilename = $uploadOverlay.find('.progress-text-filename');
    var $progressBarBar = $uploadOverlay.find('.progress-bar .bar');

    $body.on('dragover', preventDefault(stopPropagation(dragOver)));
    $dragOverlay.on('dragleave', preventDefault(stopPropagation(dragLeave)));
    $dragOverlay.on('drop', preventDefault(stopPropagation(uploadDroppedFile)));

    function dragOver () {
      $body.addClass('drag-over');
    }

    function dragLeave(e) {
      $body.removeClass('drag-over');
    }

    function uploadDroppedFile(e) {
      dragLeave(e);
      $body.addClass('uploading');

      var files = e.originalEvent.target.files || e.originalEvent.dataTransfer.files;
      if (!files.length) {
        return;
      }
      var file = files[0];
      var formData = new FormData();
      formData.append('files[]', file);

      $progressFilename.text(file.name);

      $.ajax({
        url: Drupal.settings.ombumedia.upload.url,
        type: 'POST',
        xhr: function() {
          var myXhr = $.ajaxSettings.xhr();
          if (myXhr.upload) {
            myXhr.upload.addEventListener('progress', function(e) {
              if (e.lengthComputable) {
                var loaded = e.loaded;
                var total = e.total;
                var width = ((loaded / total) * 100) + '%';
                $progressBarBar.css('width', width);
              }
            }, false);
          }
          return myXhr;
        },
        data: formData,
        cache: false,
        contentType: false,
        processData: false
      })
      .done(function(data, textStatus, jQueryXHR) {
        if (data.file && data.file.fid) {
          window.location = fileUrl(data.file.fid, 'select');
        }
      });
    }

  }
};


})(jQuery);