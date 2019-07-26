// ----------------topology--------------------
function switch_div(show) {  
    if (show == 'internet') {
        document.getElementById("topology_internet").style.display = "block";
        document.getElementById("button_internet").classList.add('active');
        document.getElementById("topology_host").style.display = "none";
        if (document.getElementById("button_host").classList.contains('active'))
            document.getElementById("button_host").classList.remove('active');
    }
    else if (show == 'host') {
        document.getElementById("topology_internet").style.display = "none";
        if (document.getElementById("button_internet").classList.contains('active'))
            document.getElementById("button_internet").classList.remove('active');
        document.getElementById("topology_host").style.display = "block";
        document.getElementById("button_host").classList.add('active');
    }
} 

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
    // ----------------internet topology------------------
    var internet_data = [{
        type: 'scattergeo',
        mode: 'markers',
        marker: {
            size: 7,
            color: '#fb8072',
            line: {
                width: 1
            }
        },
        name: 'Internet ASes',
        textposition: [
            'top right'
        ],
    }];

    var internet_layout = {
        title: 'Internet AS Locations',
        font: {
            color: 'white'
        },
        dragmode: 'zoom', 
        mapbox: {
            center: {
                lat: 38.03697222, 
                lon: -150.70916722
            }, 
            domain: {
                x: [0, 1], 
                y: [0, 1]
            }, 
            style: 'basic', 
            zoom: 1
        }, 
        margin: {
            r: 20, 
            t: 40, 
            b: 20, 
            l: 20, 
            pad: 0
        }, 
        paper_bgcolor: '#bbd2df', 
        plot_bgcolor: '#bbd2df', 
        // paper_bgcolor: '#191A1A', 
        // plot_bgcolor: '#191A1A', 
        showlegend: true,
    };
    Plotly.newPlot('topology_internet', internet_data, internet_layout, {showSendToCloud: true});         
    // ---------------------------------------------------

    // ----------------network grpah----------------------
    var network_nodes = new vis.DataSet(nodesArray);
    var network_edges = new vis.DataSet(edgesArray);
    var network_data  = {
        nodes: network_nodes,
        edges: network_edges
    };
    var network_options = {
        physics: {stabilization: true}
    };
    var network_container = document.getElementById('network');
    var network = new vis.Network(network_container, network_data, network_options);
    // ---------------------------------------------------

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

        } else if (json.type === 'topology') { // internet topology data
            addMessage("Notice", "Internet Topology data is received",
                       "red", new Date(json.data.time));
            // receive and dynamically update the topology
            var update_data = {
                // 'lon' : [[
                //     -73.57, -79.24, -123.06, -114.1, -113.28,
                //     -75.43, -63.57, -123.21, -97.13, -104.6
                // ]],
                // 'lat' : [[
                //     45.5, 43.4, 49.13, 51.1, 53.34, 45.24,
                //     44.64, 48.25, 49.89, 50.45
                // ]]
                'lon' : [json.data.longitude],
                'lat' : [json.data.latitude]
            };
            Plotly.update('topology_internet', update_data, {});
        } else if (json.type === 'eventlog') {
            addMessage("Notice", "Received eventlog",
                       "red", new Date(json.data.time));
            
            var ul = document.getElementById("ss_elem_list");
            ul.innerHTML = '';
            for (var i = 0, l = json.data.eventlogs.length; i < l; i++) {
                var eventlog = json.data.eventlogs[i];
                addMessage("eventlog", `host : ${eventlog.host}, time : ${eventlog.time}, type : ${eventlog.type}, args : ${eventlog.args}`, "blue", new Date(json.data.time));
                var li = document.createElement("li");
                li.appendChild(document.createTextNode(`${eventlog.host},${eventlog.time},${eventlog.type},${eventlog.args}`));
                li.setAttribute("id", `event_${i}`); 
                li.setAttribute("role", "option"); 
                if (eventlog.type === "API")
                    li.setAttribute("style", "display:none;");
                ul.appendChild(li);

                if (eventlog.type === "InitPeerId")
                    addNode(eventlog.args);
                else if (eventlog.type === "ConnectPeer") {
                    var from = eventlog.args.split(",")[0];
                    var to = eventlog.args.split(",")[1];
                    addEdge(from, to);
                } else if (eventlog.type === "DisconnectPeer") {
                    var from = eventlog.args.split(",")[0];
                    var to = eventlog.args.split(",")[1];
                    removeEdge(from, to);
                    removeEdge(to, from);
                } else if (eventlog.type === "BlockAppend") {
                    var peerId = eventlog.host;
                    var hash = eventlog.args.split(",")[1];
                    var prevHash = eventlog.args.split(",")[2];
                    var timestamp = eventlog.args.split(",")[3];
                    appendBlock(peerId, hash, prevHash, timestamp);
                } else if (eventlog.type === "ResultStat") {
                    var statName = eventlog.args.split(",")[0];
                    if (statName === "TotalMinedBlockNum") {
                        var curBlkNum = Number(document.getElementById('totalblk').value);
                        var blockchainLength = eventlog.args.split(",")[2];
                        curBlkNum += Number(eventlog.args.split(",")[1]);
                        document.getElementById('totalblk').value = String(curBlkNum);
                        document.getElementById('forkrate').value = (curBlkNum - Number(blockchainLength))/Number(blockchainLength);
                    }
                }
            }
            endInitialLoading();
        } else if (json.type === 'graph') {
            addMessage("Notice", "Snapshot of the blockchain graph is received",
                       "red", new Date(json.data.time));

            nodes = new vis.DataSet(json.data.nodes);
            edges = new vis.DataSet(json.data.edges);
            blockchain.setData({nodes:nodes, edges: edges});

            //------------------network graph---------------------
            network_nodes = new vis.DataSet(json.data.network_nodes);
            network_edges = new vis.DataSet(json.data.network_edges);
            network.setData({nodes:network_nodes, edges:network_edges});
            //----------------------------------------------------

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

            var confs = {};
            console.log("aaa");
            if (msg.startsWith('run')) {
                var inputs = document.getElementsByTagName('input');
                for (var i = 0; i < inputs.length; i++) {
                    var inputElement = inputs[i];
                    if (inputElement.id !== "input")
                        confs[inputElement.id] = inputElement.value;
                }
            }
            console.log(confs["txnum"]);
            console.log("bbb");

            // send the message as an ordinary text
            // connection.sendUTF(JSON.stringify( {msg: msg, conf : confs}));
            // connection.send(msg);
            connection.send(JSON.stringify( {message: msg, configure : confs }));
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
