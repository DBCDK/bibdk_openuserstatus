(function ($) {
    Drupal.behaviors.bibdk_openuserstatus = {
        attach: function(context) {
            $('.bibdk-openuserstatus-mypage-info',context).each(function(element){
                Drupal.bibdkGetUserStatus(element,context);
            });
        }
    };
    
    Drupal.setUserStatus = function (data) {
        $('.bibdk-openuserstatus-mypage-info').html(data.html);
    },
    
    Drupal.bibdkGetUserStatus =  function(element,context) {
        // save context in global var to reload later
        Drupal.settings.userStatus = context;
        // Call ajax 
        $.ajax({
            url:Drupal.settings.basePath + 'openuserstatus/getuserstatus',
            type:'POST',
            dataType:'json',
            success:Drupal.setUserStatus
        }); 
    }

}(jQuery)); 

