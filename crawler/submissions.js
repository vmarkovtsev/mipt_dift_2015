var page = require('webpage').create();
var system = require('system');
var args = system.args;
var url = 'http://informatics.mccme.ru/submits/view.php?user_id=';

page.onConsoleMessage = function(msg) {
    system.stderr.writeLine(msg);
};

page.onError = function(msg, trace) {
};

function fetch(id, index) {
  page.open(url + id + "#" + index, function (status) {
    // page.render('mccme.png');
    page.evaluate(function() {
      $("#Searchresult tr:nth-of-type(n + 2)").each(function() {
        var children = $(this).children();
        var name = $(children[2]).text();
        if ($(children[5]).text().trim() == "OK") {
          console.log(name.substr(0, name.indexOf(".")));
        }
      });
    });
    phantom.exit();
  });
}

fetch(args[1], args[2]);
