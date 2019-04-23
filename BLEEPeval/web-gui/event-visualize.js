var nodesTable = null;
var linksTable = null;
var network = null;

var nodes,edges;
var ledger = null;
var ImgDIR = 'link_network/img/';

var startButton = document.getElementById('ex1-start');
startButton.addEventListener('click', drawVisualization);


var muObserver = new MutationObserver(function(mutations) {
    mutations.forEach(function(mutation) {
        if (mutation.type == "attributes" && mutation.target.tagName == "LI"
            && mutation.attributeName=="aria-selected"
            && mutation.target.getAttribute(mutation.attributeName) == "true") {

            var eventlog = mutation.target.textContent;
            var rePattern = new RegExp(/(.+?),([0-9]+),(.+?),(.*)$/);
            var matches = eventlog.match(rePattern);
            if (matches) {
                var eventhost = matches[1];
                var eventtime = matches[2];
                var eventtype = matches[3];
                var eventargs = matches[4];
                console.log(eventtype);

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
                    sendMessage(from, to);
                } else if (eventtype === "RecvMessage") {
                    var from = eventargs.split(",")[0];
                    var to = eventargs.split(",")[1];
                    recvMessage(from, to);
                } else if (eventtype === "BlockAppend") {
                    var peerId = eventhost;
                    var hash = eventargs.split(",")[1];
                    var prevHash = eventargs.split(",")[2];
                    var timestamp = eventargs.split(",")[3];
                    appendBlock(peerId, hash, prevHash, timestamp);
                }
            }
            
            onNodeSelect(); // update event list for selected node
        }
            
    });
});

muObserver.observe(document.getElementById("ss_elem_list"), {attributes:true, subtree:true});

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
    linksTable.addColumn('number', 'width');
    linksTable.addColumn('string', 'action');
    // linksTable.addRow([3, 1, 'arrow', undefined, 1]);
    // linksTable.addRow([1, 4, 'moving-dot', undefined, 1]);
    // linksTable.addRow([1, 2, 'moving-arrows', undefined, 2]);


    // specify options
    var options = {
        'width':  '800px', 
        'height': '600px',
        'stabilize': true,
        'packages': {
            'style': 'image',
            'image': ImgDIR + 'transaction_32.png'
        }
    };

    // Instantiate our graph object.
    network = new links.Network(document.getElementById('eventvisualize'));
    
    // Add event listeners for node selection
    google.visualization.events.addListener(network, 'select', onNodeSelect);


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

function addEdge(from, to) {
    linksTable = new google.visualization.DataTable();
    linksTable.addColumn('string', 'id');
    linksTable.addColumn('string', 'from');
    linksTable.addColumn('string', 'to');
    linksTable.addColumn('string', 'style');
    linksTable.addColumn('string', 'color');
    linksTable.addColumn('number', 'width');
    linksTable.addColumn('string', 'action');
 
    if (from.startsWith("client")) {
        linksTable.addRow([from+to, from, to, 'arrow', "red", 1, 'create']); // connection is established for bidirectional communication, but use arrow for indicating who requests the connection. (i.e., 'from' requests a connection)
    }
    else {
        linksTable.addRow([from+to, from, to, 'arrow', "black", 1, 'create']); // connection is established for bidirectional communication, but use arrow for indicating who requests the connection. (i.e., 'from' requests a connection)
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
    linksTable.addColumn('number', 'width');
    linksTable.addColumn('string', 'action');
 
    // First, update edge for bi-direction
    // linksTable.addRow([from+to, from, to, 'line', undefined, 1, 'create']); // undirected graph
    // linksTable.addRow([to+from, to, from, 'line', undefined, 1, 'create']); // undirected graph


    linksTable.addRow([from+to, from, to, 'line', undefined, 1, 'delete']); // undirected graph

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
