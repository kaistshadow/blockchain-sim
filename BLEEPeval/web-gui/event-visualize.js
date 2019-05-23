var nodesTable = null;
var linksTable = null;
var network = null;

var nodes,edges;
var ledger = null;
var ImgDIR = 'link_network/img/';

var startButton = document.getElementById('ex1-start');
startButton.addEventListener('click', resetVisualization);
var toggleButton;
toggleButton = document.getElementById("toggle");


var muObserver = new MutationObserver(function(mutations) {

    var targetEvent;
    var lastEvent;
    var restart;

    mutations.forEach(function(mutation) {
        //Find the mutation occuring when the target event is selected
        if (mutation.type == "attributes" && mutation.target.tagName == "LI"
                && mutation.attributeName=="aria-selected"
                && mutation.target.getAttribute(mutation.attributeName) == "true") {
            targetEvent = mutation.target;
            
            //Check if the new event is the first displayed element of the list
            if (mutation.target == 
                    document.getElementById("ss_elem_list").querySelector('[role="option"]:not([style="display:none;"])')) {
                lastEvent = targetEvent;
                restart = true;
            }
        }

        //Find the mutation of the former selected event becoming unselected
        else if ( mutation.type == "attributes" && mutation.target.tagName == "LI"
        && mutation.attributeName == "aria-selected" && mutation.oldValue == "true" ) {
            lastEvent = mutation.target;
        }
    });

    if (targetEvent && lastEvent) {
        
        var targetEventNumber = parseInt(targetEvent.getAttribute("id").split("_")[1]);
        var lastEventNumber = parseInt(lastEvent.getAttribute("id").split("_")[1]);
        var targetReached = false;
        if (restart)
            restartEvent(targetEvent);
        
        else if (lastEventNumber < targetEventNumber) {
            while (!targetReached) {
                var nextEvent = lastEvent.nextElementSibling;
                while (nextEvent && nextEvent.getAttribute("style") === "display:none;") {
                    nextEvent = nextEvent.nextElementSibling;
                }
                if (nextEvent)
                    lastEvent = performEvent(nextEvent);
                targetReached = lastEvent == targetEvent;
           }
        }

        else if (lastEventNumber > targetEventNumber) {
            while (!targetReached) {
                lastEvent = revertEvent(lastEvent).previousElementSibling;
                while (lastEvent && lastEvent.getAttribute("style") === "display:none;") {
                    lastEvent = lastEvent.previousElementSibling;
                }
                targetReached = lastEvent == targetEvent;
            }
        }
    }
});

function restartEvent(event) {
    network.setSelection([], {unselectAll: true});
    network.nodes = [];
    network.links = [];
    network.packages = [];
    
    performEvent(event);
    network.redraw();
    return event;
}

function performEvent(event) {
    var eventlog = event.textContent;
    var rePattern = new RegExp(/(.+?),([0-9]+),(.+?),(.*)$/);
    var matches = eventlog.match(rePattern);
    if (matches) {
        var eventhost = matches[1];
        var eventtime = matches[2];
        var eventtype = matches[3];
        var eventargs = matches[4];
        console.log("Performing " + eventtype);

        if (eventtype === "InitPeerId")
            addNode(eventargs);
        else if (eventtype === "ConnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            addEdge(from, to);
        }
        else if (eventtype === "DisconnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];

            removeEdge(from, to);
            removeEdge(to, from);
        } else if (eventtype === "UnicastMessage") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            var hashId = eventargs.split(",")[3];
            sendMessage(from, to, hashId);
        } else if (eventtype === "RecvMessage") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            var hashId = eventargs.split(",")[3];
            recvMessage(from, to, hashId);
        } else if (eventtype === "BlockAppend") {
            var peerId = eventhost;
            var hash = eventargs.split(",")[1];
            var prevHash = eventargs.split(",")[2];
            var timestamp = eventargs.split(",")[3];
            appendBlock(peerId, hash, prevHash, timestamp);
        }
    }
    onNodeSelect(); // update event list for selected node
    return event;
}

function revertEvent(event) {
    var eventlog = event.textContent;
    var rePattern = new RegExp(/(.+?),([0-9]+),(.+?),(.*)$/);
    var matches = eventlog.match(rePattern);
    if (matches) {
        var eventhost = matches[1];
        var eventtime = matches[2];
        var eventtype = matches[3];
        var eventargs = matches[4];
        console.log("Reverting " + eventtype);

        if (eventtype === "InitPeerId")
            removeNode(eventargs);
        else if (eventtype === "ConnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            removeEdge(from, to);
            removeEdge(to, from);
        } else if (eventtype === "DisconnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            addEdge(from, to);
        } else if (eventtype === "UnicastMessage") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            var hashId = eventargs.split(",")[3];
            unsendMessage(from, to, hashId);
        } else if (eventtype === "RecvMessage") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            var hashId = eventargs.split(",")[3];
            unrecvMessage(from, to, hashId);
        } else if (eventtype === "BlockAppend") {
            var peerId = eventhost;
            var hash = eventargs.split(",")[1];
            var prevHash = eventargs.split(",")[2];
            var timestamp = eventargs.split(",")[3];
            removeBlock(peerId, hash, prevHash, timestamp);
        }
    }
    onNodeSelect(); // update event list for selected node
    return event;
}

muObserver.observe(document.getElementById("ss_elem_list"), {attributes:true, subtree:true, attributeOldValue:true});

google.load("visualization", "1");

// Set callback to run when API is loaded
google.setOnLoadCallback(drawVisualization);
// Called when the Visualization API is loaded.
// Called when the Visualization API is loaded.
function drawVisualization() {
    // Create and populate a data table.
    nodesTable = new google.visualization.DataTable();
    nodesTable.addColumn('string', 'id');
    nodesTable.addColumn('number', 'value');
    nodesTable.addColumn('string', 'text');
    nodesTable.addColumn('string', 'style');
    nodesTable.addColumn('string', 'group');
    nodesTable.addColumn('string', 'image');

    // create client

    // nodesTable.addRow(['client', undefined, 'client', 'image', 'group_client', ImgDIR + 'User-Coat-Blue-icon.png']);


    linksTable = new google.visualization.DataTable();
    linksTable.addColumn('string', 'id');
    linksTable.addColumn('string', 'from');
    linksTable.addColumn('string', 'to');
    linksTable.addColumn('string', 'style');
    linksTable.addColumn('string', 'color');
    linksTable.addColumn('string', 'highlightColor');
    linksTable.addColumn('number', 'width');
    linksTable.addColumn('string', 'action');
    // linksTable.addRow([3, 1, 'arrow', undefined, 1]);
    // linksTable.addRow([1, 4, 'moving-dot', undefined, 1]);
    // linksTable.addRow([1, 2, 'moving-arrows', undefined, 2]);


    // Specify options using the dimensions of the network if it has already been drawn
    var options = {
        'width': network ? network.frame.style.width : '800px', 
        'height': network ? network.frame.style.height : '600px',
        'stabilize': true,
        'links': {
            "length": 100,   // px
            "color": "#cccccc",
            "highlightColor": "#343434"
        },
        'nodes': {
            'distance': 100
        },
        'packages': {
            'style': 'image',
            'image': ImgDIR + 'transaction_32.png'
        }
    };

    // Instantiate our graph object.
    network = new links.Network(document.getElementById('eventvisualize'));

    // Add event listeners for node selection
    google.visualization.events.addListener(network, 'select', onNodeSelect);

    // Add event listeners for toggling network animation physics
    toggleButton.addEventListener('click', function() {network.toggle()});

    // Draw our graph with the created data and options 
    network.draw(nodesTable, linksTable, options);

    // start generating random emails
    // timeout();


    // Draw ledger event visualization
    var container = document.getElementById('ledgereventvisualize');

    var nodesArray = [
        {id:"0000000000", label:"genesis"}
    ];
    var edgesArray = [
    ];
    nodes = new vis.DataSet(nodesArray);
    edges = new vis.DataSet(edgesArray);
    var data = {
        nodes: nodes,
        edges: edges
    };
    var ledgerplotoptions = {
        layout: {
            hierarchical: {
                direction: "UD",
                sortMethod: "directed",
                levelSeparation: 30,
                nodeSpacing : 100
            }
        },
        interaction: {dragNodes :false},
        physics: {
            enabled: false,
        },
        nodes : {
            shape: "box",
            size: 50
        },
        edges : {
            length: 1
        }
    };

    ledger = new vis.Network(container, data, ledgerplotoptions);

}

function resetVisualization() {
    // Reset network event visualization
    network.setSelection([], {unselectAll: true});
    network.nodes = [];
    network.links = [];
    network.packages = [];

    // Reset ledger event visualization
    var nodesArray = [
        {id:"0000000000", label:"genesis"}
    ];
    var edgesArray = [
    ];
    nodes = new vis.DataSet(nodesArray);
    edges = new vis.DataSet(edgesArray);
    var data = {
        nodes: nodes,
        edges: edges
    };
    ledger.setData(data);
}

function timeout() {
    sendTransaction('client', 'bleep0');
    
    var delay = Math.round(100 + Math.random() * 1000);
    setTimeout(timeout, delay);
}

function sendTransaction(from, to) {
    try {

        var packagesTable = new google.visualization.DataTable();
        packagesTable.addColumn('string', 'from');
        packagesTable.addColumn('string', 'to');        
        packagesTable.addColumn('number', 'duration');        
        packagesTable.addRow([from, to, 2]);
        network.addPackages(packagesTable);
    }
    catch(err) {
        alert(err);
    }
}      

function recvMessage(from, to) {
    try {
        var packagesTable = new google.visualization.DataTable();
        packagesTable.addColumn('string', 'id');
        packagesTable.addColumn('string', 'from');
        packagesTable.addColumn('string', 'to');        
        packagesTable.addColumn('number', 'progress');        
        packagesTable.addColumn('number', 'duration');        
        packagesTable.addColumn('string', 'action');        
        packagesTable.addRow([from+to, undefined, undefined, undefined, undefined, 'delete']);
        network.addPackages(packagesTable);

        packagesTable.removeRow(0);
        packagesTable.addRow([from+to, from, to, undefined, 1, 'create']);
        network.addPackages(packagesTable);
    }
    catch(err) {
        alert(err);
    }
}

function unrecvMessage(from, to, hashId) {
    try {
        var packagesTable = new google.visualization.DataTable();
        packagesTable.addColumn('string', 'id');
        packagesTable.addColumn('string', 'from');
        packagesTable.addColumn('string', 'to');        
        packagesTable.addColumn('number', 'progress');        
        packagesTable.addColumn('number', 'duration');        
        packagesTable.addColumn('string', 'action');        
        packagesTable.addRow([from+to+hashId, from, to, 0.001, undefined, 'create']);
        network.addPackages(packagesTable);
    }
    catch(err) {
        alert(err);
    }
}

function sendMessage(from, to, hashId) {
    try {
        var packagesTable = new google.visualization.DataTable();
        packagesTable.addColumn('string', 'id');
        packagesTable.addColumn('string', 'from');
        packagesTable.addColumn('string', 'to');        
        packagesTable.addColumn('number', 'progress');        
        packagesTable.addColumn('number', 'duration');        
        packagesTable.addColumn('string', 'action');        
        packagesTable.addRow([from+to, from, to, 0.001, undefined, 'create']);
        network.addPackages(packagesTable);
    }
    catch(err) {
        alert(err);
    }
}

function sendMessage(from, to) {
    try {
        var packagesTable = new google.visualization.DataTable();
        packagesTable.addColumn('string', 'id');
        packagesTable.addColumn('string', 'from');
        packagesTable.addColumn('string', 'to');        
        packagesTable.addColumn('number', 'progress');        
        packagesTable.addColumn('number', 'duration');      
        packagesTable.addColumn('string', 'action');          
        packagesTable.addRow([from+to, from, to, 0.001 ,undefined, 'create']);
        network.addPackages(packagesTable);
    }
    catch(err) {
        alert(err);
    }
}

function unsendMessage(from, to, hashId) {
    try {
        var packagesTable = new google.visualization.DataTable();
        packagesTable.addColumn('string', 'id');
        packagesTable.addColumn('string', 'from');
        packagesTable.addColumn('string', 'to');
        packagesTable.addColumn('number', 'progress');
        packagesTable.addColumn('number', 'duration');
        packagesTable.addColumn('string', 'action');
        packagesTable.addRow([from+to+hashId, from, to, undefined ,undefined, 'delete']);
        network.addPackages(packagesTable);
    }
    catch(err) {
        alert(err);
    }
}

function addNode(nodeid) {
    // Create and populate a data table.
    nodesTable = new google.visualization.DataTable();
    nodesTable.addColumn('string', 'id');
    nodesTable.addColumn('number', 'value');
    nodesTable.addColumn('string', 'text');
    nodesTable.addColumn('string', 'style');
    nodesTable.addColumn('string', 'group');
    nodesTable.addColumn('string', 'image');
    // nodesTable.addColumn('number', 'x');
    // nodesTable.addColumn('number', 'y');


    // create node
    nodesTable.addRow([nodeid, undefined, nodeid, 'rect', 'group_node', undefined]);
    network.addNodes(nodesTable);

    // specify options
    // var options = {
    //     'width':  '800px', 
    //     'height': '600px',
    //     'stabilize': true,
    //     'packages': {
    //         'style': 'image',
    //         'image': ImgDIR + 'transaction_32.png'
    //     }
    // };

    network.redraw();
}

function removeNode(nodeid) {
    try {
        nodesTable = new google.visualization.DataTable();
        nodesTable.addColumn('string', 'id');
        nodesTable.addColumn('number', 'value');
        nodesTable.addColumn('string', 'text');
        nodesTable.addColumn('string', 'style');
        nodesTable.addColumn('string', 'group');
        nodesTable.addColumn('string', 'image');
        nodesTable.addColumn('string', 'action');
        nodesTable.addRow([nodeid, undefined, nodeid, 'rect', 'group_node', undefined, "delete"]);
        network.addNodes(nodesTable);
    }
    catch(err) {
        alert(err);
    }

    network.redraw();
};

function addEdge(from, to) {
    linksTable = new google.visualization.DataTable();
    linksTable.addColumn('string', 'id');
    linksTable.addColumn('string', 'from');
    linksTable.addColumn('string', 'to');
    linksTable.addColumn('string', 'style');
    linksTable.addColumn('string', 'color');
    linksTable.addColumn('string', 'highlightColor');
    linksTable.addColumn('number', 'width');
    linksTable.addColumn('string', 'action');
 
    if (from.startsWith("client")) {
        linksTable.addRow([from+to, from, to, 'arrow', "red", "red", 1, 'create']); // connection is established for bidirectional communication, but use arrow for indicating who requests the connection. (i.e., 'from' requests a connection)
    }
    else {
        linksTable.addRow([from+to, from, to, 'arrow', undefined, undefined, 1, 'create']); // connection is established for bidirectional communication, but use arrow for indicating who requests the connection. (i.e., 'from' requests a connection)
    }

    network.addLinks(linksTable);
}

function removeEdge(from, to) {
    linksTable = new google.visualization.DataTable();
    linksTable.addColumn('string', 'id');
    linksTable.addColumn('string', 'from');
    linksTable.addColumn('string', 'to');
    linksTable.addColumn('string', 'style');
    linksTable.addColumn('string', 'color');
    linksTable.addColumn('string', 'hightlightColor');
    linksTable.addColumn('number', 'width');
    linksTable.addColumn('string', 'action');
 
    // First, update edge for bi-direction
    // linksTable.addRow([from+to, from, to, 'line', undefined, undefined, 1, 'create']); // undirected graph
    // linksTable.addRow([to+from, to, from, 'line', undefined, undefined, 1, 'create']); // undirected graph


    linksTable.addRow([from+to, from, to, 'line', undefined, undefined, 1, 'delete']); // undirected graph

    // (catch err because linksTable returns error when we try to remove non-existed edge)
    try {
        network.addLinks(linksTable);
    } catch(err) {

    }
}

function appendBlock(peerId, hash, prevHash, timestamp) {
    if (prevHash === "0000000000") {
        // (catch err because dataSet returns error when we try to add existed node)
        try {
            nodes.add({id:prevHash, label:"genesis"});
        } catch(err) {

        }
    }
    // (catch err because dataSet returns error when we try to add existed node)
    try {
        nodes.add({id:hash, label:timestamp});
    } catch(err) {
    }
    // (catch err because dataSet returns error when we try to add existed node)
    try {
        nodes.add({id:prevHash, label:timestamp});
    } catch(err) {
    }

    // (catch err because dataSet returns error when we try to add existed node)
    try {
        edges.add({id:prevHash+hash, from:prevHash, to:hash});
    } catch(err) {
    }

    // add node pointer
    try {

    } catch(err) {}


    edges.remove(peerId);
    try {
        edges.add({id:peerId, from:hash, to:peerId});
    } catch(err) {
    }
    try {
        nodes.update({id:peerId, label:peerId});
    } catch(err) {}

    // nodes.remove({id:peerId});    
    // edges.update({id:peerId, from:hash, to:peerId});
    // nodes.add({id:peerId, label:peerId});


}

function removeBlock(peerId, hash, prevHash, timestamp) {
    
    edges.remove(peerId);
    try {
        edges.add({id:peerId, from:prevHash, to:peerId});
    } catch(err) {
    }
    try {
        nodes.update({id:peerId, label:peerId});
    } catch(err) {}

    if ( edges.distinct("from").find(from => from == hash) == undefined ) {
        //If no edge start from node "hash", delete node "hash"
        nodes.remove(hash);
        edges.remove(prevHash+hash);

        if ( edges.get({filter: edge => edge.to == prevHash}).length == 0
        && edges.get({filter: edge => edge.from == prevHash}).length == 1 ) {
            //If prevHash is the first block and has no child
            nodes.remove([prevHash, peerId]);
        }
    }

}

function endInitialLoading() {
    network.endFirstRun = true;
}

function onNodeSelect() {
    var sel = network.getSelection();
    var nodeid;
    if (sel[0]) {
        nodeid = network.nodes[sel[0].row]['id'];
    }
    else
        nodeid = "none. Node is not selected";

    var span = document.getElementById("node_API_event");
    span.innerHTML = `API call history for ${nodeid}<br>Format:&lt;API called hostid&gt;,&lt;API called time&gt;,API,&lt;API name&gt;,&lt;API args(in brief form)&gt;`;

    var ul = document.getElementById("node_API_event_list");
    ul.innerHTML = ''; // reset 

    var items = document.getElementById("ss_elem_list").getElementsByTagName("li");
    var node_event_count = 0;
    for (var i = 0; i < items.length; i++) {
        var item = items[i];
        
        if (item.innerHTML.includes(",API,") && item.innerHTML.startsWith(nodeid)) {
            var li = document.createElement("li");
            li.appendChild(document.createTextNode(item.innerHTML));
            li.setAttribute("id", 'node_API_event_${node_event_count}');
            li.setAttribute("role", "option"); 
            ul.appendChild(li);
            node_event_count++;
        }
        if (item.hasAttribute("aria-selected"))
            break;
    }

    // scroll down the event list
    $('#node_API_event_list').animate({scrollTop: $('#node_API_event_list').prop("scrollHeight")}, 0 /*duration*/);
}
