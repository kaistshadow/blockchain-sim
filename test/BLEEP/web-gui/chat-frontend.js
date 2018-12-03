$(function () {
    "use strict";

    // for network
    var nodesArray = [
        {
            id:"a",
            label:"A"
        },
        {
            id:"b",
            label:"B"
        }
    ];
    var edgesArray = [
        {
            from:"a",
            to:"b"
        }
    ];
    var nodes = new vis.DataSet(nodesArray);
    var edges = new vis.DataSet(edgesArray);
    var data = {
        nodes: nodes,
        edges: edges
    };    
    var options = {
        layout: {
            hierarchical: {
                direction: "UD",
                sortMethod: "directed",
                levelSeparation: 30,
                nodeSpacing : 150
            }
        },
        interaction: {dragNodes :false},
        physics: {
            enabled: false
        },
        nodes : {
            shape: "box",
            size: 50
        },
        edges : {
            length: 1
        }
    };
    var container = document.getElementById('blockchain');
    var blockchain = new vis.Network(container, data, options);



    // for better performance - to avoid searching in DOM
    var content = $('#content');
    var input = $('#input');
    var status = $('#status');

    // my connection index
    var myIndex = -1;
    

    // if user is running mozilla then use it's built-in WebSocket
    window.WebSocket = window.WebSocket || window.MozWebSocket;

    // if browser doesn't support WebSocket, just show some notification and exit
    if (!window.WebSocket) {
        content.html($('<p>', { text: 'Sorry, but your browser doesn\'t '
                                    + 'support WebSockets.'} ));
        input.hide();
        $('span').hide();
        return;
    }

    function url_domain(data) {
        var    a      = document.createElement('a');
        a.href = data;
        return a.host;
    }
    var domain = url_domain(window.location.href);

    // open connection
    var connection = new WebSocket(`ws://${domain}`);

    connection.onopen = function () {
        // first we want users to enter their names
        input.removeAttr('disabled');
        // status.text('Choose name:');
        status.text('Type command:');
    };

    connection.onerror = function (error) {
        // just in there were some problems with conenction...
        content.html($('<p>', { text: 'Sorry, but there\'s some problem with your '
                                    + 'connection or the server is down.' } ));
    };

    // most important part - incoming messages
    connection.onmessage = function (message) {
        // try to parse JSON message. Because we know that the server always returns
        // JSON this should work without any problem but we should make sure that
        // the massage is not chunked or otherwise damaged.
        try {
            var json = JSON.parse(message.data);
        } catch (e) {
            console.log('This doesn\'t look like a valid JSON: ', message.data);
            return;
        }

        // NOTE: if you're not sure about the JSON structure
        // check the server source code above
        // if (json.type === 'color') { // first response from the server with user's color
        //     myColor = json.data;
        //     status.text(myName + ': ').css('color', myColor);
        //     input.removeAttr('disabled').focus();
        //     // from now user can start sending messages
        // } 
        if (json.type === 'history') { // entire message history
            // insert every single message to the chat window
            for (var i=0; i < json.data.length; i++) {
                addMessage(json.data[i].author, json.data[i].text,
                           json.data[i].color, new Date(json.data[i].time));
            }
        } 
        else if (json.type === 'message') { // it's a single message
            input.removeAttr('disabled'); // let the user write another message
            addMessage(json.data.author, json.data.text,
                       json.data.color, new Date(json.data.time));

        } else if (json.type === 'graph') {
            addMessage("Notice", "Snapshot of the blockchain graph is received",
                       "red", new Date(json.data.time));
            nodes = new vis.DataSet(json.data.nodes);
            edges = new vis.DataSet(json.data.edges);
            blockchain.setData({nodes:nodes, edges: edges});
            for (const [key, value] of Object.entries(json.data.timestamps)) {
                addMessage("simul-timelog", `Node ${key}'s current virtualTime=${value}`, "blue", new Date(json.data.time));
                if (key >= 10)
                    break;
            }
        } else if (json.type === 'status') {
            if (json.data.operator === -1) {
                status.text("Type command:");
            }
            else if (json.data.operator !== myIndex) {
                status.text("Type command (Now, other user is processing):");
            }
        } else if (json.type === 'userindex') {
            myIndex = json.data;
        }
        else {
            console.log('Hmm..., I\'ve never seen JSON like this: ', json);
        }
    };

    /**
     * Send mesage when user presses Enter key
     */
    input.keydown(function(e) {
        if (e.keyCode === 13) {
            var msg = $(this).val();
            if (!msg) {
                return;
            }
            // send the message as an ordinary text
            connection.send(msg);
            $(this).val('');
            // disable the input field to make the user wait until server
            // sends back response
            input.attr('disabled', 'disabled');
        }
    });

    /**
     * This method is optional. If the server wasn't able to respond to the
     * in 3 seconds then show some error message to notify the user that
     * something is wrong.
     */
    setInterval(function() {
        if (connection.readyState !== 1) {
            status.text('Error');
            input.attr('disabled', 'disabled').val('Unable to comminucate '
                                                 + 'with the WebSocket server.');
        }
    }, 3000);

    /**
     * Add message to the chat window
     */
    function addMessage(author, message, color, dt) {
        content.prepend('<p><span style="color:' + color + '">' + author + '</span> @ ' +
             + (dt.getHours() < 10 ? '0' + dt.getHours() : dt.getHours()) + ':'
             + (dt.getMinutes() < 10 ? '0' + dt.getMinutes() : dt.getMinutes()) + ':'
             + (dt.getSeconds() < 10 ? '0' + dt.getSeconds() : dt.getSeconds())
             + ': ' + message + '</p>');
    }
});
