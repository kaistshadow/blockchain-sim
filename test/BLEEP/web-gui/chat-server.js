// http://ejohn.org/blog/ecmascript-5-strict-mode-json-and-more/
"use strict";

// Optional. You will see this name in eg. 'ps' or 'top' command
process.title = 'node-chat';

// Port where we'll run the websocket server
var webSocketsServerPort = 1337;

// websocket and http servers
var webSocketServer = require('websocket').server;
var http = require('http');

/**
 * Global variables
 */
// current operator id
var operatorIndex = -1;

// latest 100 messages
var history = [ ];
// list of currently connected clients (users)
var clients = [ ];
var globalClientId = 0;
// var connectionIndex = new Map();
var finalhandler = require('finalhandler');
var serveStatic = require('serve-static');

// blockchain infos
var blockchainBlocks = [];
var blockchainEdges = [];

var serve = serveStatic("./");
/**
 * Helper function for escaping input strings
 */
function htmlEntities(str) {
    return String(str).replace(/&/g, '&amp;').replace(/</g, '&lt;')
                      .replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}

// Array with some colors
var colors = [ 'red', 'green', 'blue', 'magenta', 'purple', 'plum', 'orange' ];
// ... in random order
colors.sort(function(a,b) { return Math.random() > 0.5; } );

/**
 * HTTP server
 */
var server = http.createServer(function(request, response) {
    var done = finalhandler(request, response);
    serve(request, response, done);
    // Not important for us. We're writing WebSocket server, not HTTP server
});
server.listen(webSocketsServerPort, function() {
    console.log((new Date()) + " Server is listening on port " + webSocketsServerPort);
});

/**
 * WebSocket server
 */
var wsServer = new webSocketServer({
    // WebSocket server is tied to a HTTP server. WebSocket request is just
    // an enhanced HTTP request. For more info http://tools.ietf.org/html/rfc6455#page-6
    httpServer: server
});

function sendSnapshot(connection, nodenum) {
    // send log file content
    var fs = require('fs');
    var datadir = "centralized-broadcast-async-datadir";
    var shadowPlugin = "PEER_POWCONSENSUS";
    var rePattern = new RegExp(/.*Block idx=([0-9]+),.*,Block hash=\[\[([0-9a-fA-F]{14}).+\]\[.+\]\],Prev Block hash=\[\[([0-9a-fA-F]{14}).+\]\[.+\]\],Timestamp=\[(.+)\],Difficulty=.+/);

    var vis = require('vis');
    // var nodes = new vis.DataSet();
    // var edges = new vis.DataSet();
    var nodesArray = [];
    var edgesArray = [];
    var timestampsDict = {};
    var curDate = new Date();
    var curTime = (curDate).getTime();
    var snapshottime = (curDate.getHours() < 10 ? '0' + curDate.getHours() : curDate.getHours()) + ':'
            + (curDate.getMinutes() < 10 ? '0' + curDate.getMinutes() : curDate.getMinutes()) + ':'
            + (curDate.getSeconds() < 10 ? '0' + curDate.getSeconds() : curDate.getSeconds());


    var genesisBlk = {id:"00000000000000", label:"genesis" + ", " + snapshottime};
    if (!blockchainBlocks.find( x => {return x.id === "00000000000000";}))
        blockchainBlocks.push(genesisBlk);
    // nodes.update([{id:"00000000000000", label:"genesis"}]);
    for (let i = 0; i < nodenum; i++) {
        var logfile = `../${datadir}/hosts/bleep${i}/stdout-bleep${i}.${shadowPlugin}.1000.log`;
        if (!fs.existsSync(logfile))
            continue;
        var lines = fs.readFileSync(logfile).toString().split('\n');
        var blocks = {};
        for (let line of lines) {
            var matches = line.match(rePattern);
            if (matches) {
                var block_idx = matches[1];
                var block_hash = matches[2];
                var prev_block_hash = matches[3];
                var timestamp = matches[4];
                blocks[block_idx] = [block_hash, prev_block_hash, timestamp];
            }
            else if (line.startsWith("Time:")) {
                // timestampsArray.push({virtualtime:line, node:i});
                timestampsDict[i] = line;
            }
        }
        
        var keys = Object.keys(blocks);
        keys.sort((a,b) => a-b);
        for (var idx of keys) {
            var block_hash = blocks[idx][0];
            var prev_block_hash = blocks[idx][1];
            var timestamp = blocks[idx][2];

            if (!blockchainBlocks.find( x => {return x.id === block_hash;}))
                blockchainBlocks.push({id:block_hash,label:timestamp + ", " + snapshottime});
            if (!blockchainEdges.find( x => {return x.id === prev_block_hash+block_hash;}))
                blockchainEdges.push({id:prev_block_hash+block_hash,from:prev_block_hash, to:block_hash});
        }
        
        // var lastblockhash = blocks[keys.pop()][0];
        // nodes.update([{id:`bleep${i}`, label:`bleep${i}`}]);
        // edges.update([{id:lastblockhash+`bleep${i}`, from:lastblockhash, to:`bleep${i}`}]);
    }
    // nodesArray = nodes.get();
    // edgesArray = edges.get();

    var graphObj = {
        time: curTime,
        nodes: blockchainBlocks,
        edges: blockchainEdges,
        // nodes: nodesArray,
        // edges: edgesArray,
        timestamps: timestampsDict
    };
    // broadcast message to requested connection
    var json = JSON.stringify({ type:'graph', data: graphObj });
    connection.sendUTF(json);
}

// This callback function is called every time someone
// tries to connect to the WebSocket server
wsServer.on('request', function(request) {
    console.log((new Date()) + ' Connection from origin ' + request.origin + '.');

    // accept connection - you should check 'request.origin' to make sure that
    // client is connecting from your website
    // (http://en.wikipedia.org/wiki/Same_origin_policy)
    var connection = request.accept(null, request.origin); 
    // we need to know client index to remove them on 'close' event
    // var index = clients.push(connection) - 1;   // PROBLEM : this variable is not well-updated
    // connectionIndex.set(connection, clients.push(connection) - 1);   // PROBLEM : this variable is not well-updated
    if (globalClientId == Number.MAX_SAFE_INTEGER) {
        // 
        consol.log((new Date()) + ' Connection rejected. globalCounter is approached to maximum value. Need server reboot.');
        return;
    }
    var index = globalClientId++;
    
    clients.push({id:index, conn : connection});
    // var userName = false;

    console.log((new Date()) + ' Connection accepted.');

    var proc = null;
    var snapshotIntervalId;

    // send back chat history
    if (history.length > 0) {
        connection.sendUTF(JSON.stringify( { type: 'history', data: history} ));
    }
    connection.sendUTF(JSON.stringify( {type: 'userindex', data: index}));
    connection.sendUTF(JSON.stringify( {type:'status', data: {operator:operatorIndex }  } ));
    

    // user sent some message
    connection.on('message', function(message) {
        if (message.type === 'utf8') { // accept only text
            if ((message.utf8Data === 'run' || message.utf8Data.startsWith('run ')) && operatorIndex != -1) {
                var obj = {
                    time: (new Date()).getTime(),
                    text: "Unable to run : Now processing another user's request",
                    author: "Notice",
                    color: 'red'
                };
                var json = JSON.stringify({ type:'message', data: obj });
                connection.sendUTF(json);
                return;
            }
            else if ((message.utf8Data === 'run' || message.utf8Data.startsWith('run ')) && operatorIndex == -1) {
                const child_process = require('child_process');
                var nodenum = 500;

                console.log((new Date()) + ' Received Message : ' + message.utf8Data);

                var cmd_tokens = message.utf8Data.split(" ");
                for (var cmd_idx in cmd_tokens) {
                    if ("--nodenum" === cmd_tokens[cmd_idx]) {
                        nodenum = parseInt(cmd_tokens[parseInt(cmd_idx)+1]);
                    }
                }
                console.log((new Date()) + ' subprocess with command : ' + 'python test-centralized-broadcast-async.py --nodenum ' + nodenum);                
                console.log('current directory: ' + process.cwd());
                proc = child_process.spawn('python', ['test-centralized-broadcast-async.py', "--nodenum", nodenum], {cwd:"../"});

                proc.on("exit", function(exitCode) {
                    console.log((new Date()) + ' process exited with code ' + exitCode);
                    var obj = {
                        time: (new Date()).getTime(),
                        text: `run command operated by user${operatorIndex} is terminated`,
                        author: "Notice",
                        color: 'red'
                    };
                    for (var i=0; i < clients.length; i++) {
                        clients[i].conn.sendUTF(JSON.stringify({ type:'message', data: obj }));
                    }

                    // set operator as none
                    operatorIndex = -1;
                    for (var i=0; i < clients.length; i++) {
                        clients[i].conn.sendUTF(JSON.stringify({ type:'status', data: {operator:operatorIndex} }));
                    }
                    proc = null;
                    clearInterval(snapshotIntervalId);
                    sendSnapshot(connection, nodenum);
                    blockchainBlocks = [];
                    blockchainEdges = [];
                });

                proc.stdout.on("data", function(chunk) {
                    // console.log('received chunk ' + chunk);
                });

                proc.stdout.on("end", function() {
                    console.log((new Date()) + " finished collecting data chunks from stdout");
                });
                snapshotIntervalId = setInterval(sendSnapshot, 1000, connection, nodenum);


                // set user as operator
                operatorIndex = index;
                var json = JSON.stringify({ type:'status', data: {operator:operatorIndex } });
                console.log((new Date()) + ' Operator is selected as: ' + operatorIndex);
                for (var i=0; i < clients.length; i++) {
                    clients[i].conn.sendUTF(json);
                }
            }
            else if (message.utf8Data === 'stop') {
                if (proc) {
                    proc.kill();
                    clearInterval(snapshotIntervalId);
                }
                else 
                    return;
                // set operator as none
                // operatorIndex = -1;
                // for (var i=0; i < clients.length; i++) {
                //     clients[i].conn.sendUTF(JSON.stringify({ type:'status', data: {operator:operatorIndex }}));
                // }
            }

            // if (userName === false) { // first message sent by user is their name
            //     // remember user name
            //     userName = htmlEntities(message.utf8Data);
            //     // get random color and send it back to the user
            //     userColor = colors.shift();
            //     connection.sendUTF(JSON.stringify({ type:'color', data: userColor }));
            //     console.log((new Date()) + ' User is known as: ' + userName
            //                 + ' with ' + userColor + ' color.');

            // } else { // log and broadcast the message
            //     console.log((new Date()) + ' Received Message from '
            //                 + userName + ': ' + message.utf8Data);
                
            //     // we want to keep history of all sent messages
            //     var obj = {
            //         time: (new Date()).getTime(),
            //         text: htmlEntities(message.utf8Data),
            //         author: userName,
            //         color: userColor
            //     };
            //     history.push(obj);
            //     history = history.slice(-100);

            //     // broadcast message to all connected clients
            //     var json = JSON.stringify({ type:'message', data: obj });
            //     for (var i=0; i < clients.length; i++) {
            //         clients[i].sendUTF(json);
            //     }
            // }
            // log and broadcast the message
            // console.log((new Date()) + ' Received Message from '
            //             + userName + ': ' + message.utf8Data);
            
            // we want to keep history of all sent messages
            var obj = {
                time: (new Date()).getTime(),
                text: htmlEntities(message.utf8Data),
                author: "user"+index,
                color: 'red'
            };
            history.push(obj);
            history = history.slice(-100);

            // broadcast message to all connected clients
            var json = JSON.stringify({ type:'message', data: obj });
            for (var i=0; i < clients.length; i++) {
                clients[i].conn.sendUTF(json);
            }
        }
    });

    // user disconnected
    connection.on('close', function(connection) {
        console.log((new Date()) + " Peer "
                    + connection.remoteAddress + " disconnected.");
        // remove user from the list of connected clients
        // clients.splice(connectionIndex.get(connection), 1);
        clients.filter( x => {return x.id !== index;});
        // update overall connection index
        // for (var index in clients) 
        //     connectionIndex.set(clients[index], index);
        // push back user's color to be reused by another user
        // colors.push(userColor);

        if (proc) {
            proc.kill();
            clearInterval(snapshotIntervalId);
        }
        // for (var i=0; i < clients.length; i++) {
        //     console.log("userindex update for" + i);
        //     clients[i].sendUTF(JSON.stringify( {type: 'userindex', data: i}));
        // }
    });

});
