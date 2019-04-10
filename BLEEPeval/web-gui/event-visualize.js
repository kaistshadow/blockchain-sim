var nodesTable = null;
var linksTable = null;
var network = null;
var ImgDIR = 'link_network/img/';

var startButton = document.getElementById('ex1-start');
startButton.addEventListener('click', drawVisualization);


var muObserver = new MutationObserver(function(mutations) {
    mutations.forEach(function(mutation) {
        if (mutation.type == "attributes" && mutation.target.tagName == "LI"
            && mutation.attributeName=="aria-selected"
            && mutation.target.getAttribute(mutation.attributeName) == "true") {

            var eventlog = mutation.target.textContent;
            var rePattern = new RegExp(/([0-9]+),(.+?),(.*)$/);
            var matches = eventlog.match(rePattern);
            if (matches) {
                var eventtime = matches[1];
                var eventtype = matches[2];
                var eventargs = matches[3];
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
                }
                
            }
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

    // Draw our graph with the created data and options 
    network.draw(nodesTable, linksTable, options);

    // start generating random emails
    // timeout();
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
        linksTable.addRow([from+to, from, to, 'arrow', "red", 1, 'create']); // undirected graph, but use arrow for fancy visualization
        linksTable.addRow([to+from, to, from, 'arrow', "red", 1, 'create']); // undirected graph, but use arrow for fancy visualization
    }
    else {
        linksTable.addRow([from+to, from, to, 'arrow', "black", 1, 'create']); // undirected graph, but use arrow for fancy visualization
        linksTable.addRow([to+from, to, from, 'arrow', "black", 1, 'create']); // undirected graph, but use arrow for fancy visualization
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
