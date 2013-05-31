(function($) {
    Drupal.changePickupLib = function(element) {

        var oid = element.data('oid');
        var aid = element.val();
        var caid = element.data('caid');

        if(!oid || !aid || !caid) {
            console.log('error: No oid, aid or caid found');
        }

        var request = $.ajax({
            url: Drupal.settings.basePath + 'userstatus/pickuplib',
            type: 'POST',
            data: {
                oid: oid,
                aid: aid,
                caid: caid
            },
            dataType: 'json',
            success: Drupal.userstatusResponse
        });
    };

    Drupal.userstatusResponse = function(data) {
        console.log(data);

        if(data.error) {
            var error = data.error;
            alert(error.msg);
        }
    };

    Drupal.addThrobber = function(element) {
        $(element).addClass('ajax-progress');
        $(element).html('<span class="throbber">&nbsp;</span>');
    };


    Drupal.removeThrobber = function(element) {

    };

    Drupal.deactivateSelects = function(){

    }

    Drupal.activateSelects = function() {

    }

    Drupal.behaviors.userstatus = {
        attach: function(context) {
            $('.form-select', context).change(function() {
                var element = $(this);
                //Drupal.addThrobber(element);
                Drupal.changePickupLib(element);
            });
        }
    };
}(jQuery));