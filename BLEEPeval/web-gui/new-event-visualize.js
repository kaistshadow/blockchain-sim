var n_nodesTable = null;
var n_linksTable = null;
var n_network = null;

var n_nodes,n_edges;
var n_ledger = null;
var n_ImgDIR = 'link_network/img/';


var n_muObserver = new MutationObserver(function(mutations) {

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
            n_restartEvent(targetEvent);

        else if (lastEventNumber < targetEventNumber) {
            while (!targetReached) {
                var nextEvent = lastEvent.nextElementSibling;
                while (nextEvent && nextEvent.getAttribute("style") === "display:none;") {
                    nextEvent = nextEvent.nextElementSibling;
                }
                if (nextEvent)
                    lastEvent = n_performEvent(nextEvent);
                targetReached = lastEvent == targetEvent;
           }
        }

        else if (lastEventNumber > targetEventNumber) {
            while (!targetReached) {
                lastEvent = n_revertEvent(lastEvent).previousElementSibling;
                while (lastEvent && lastEvent.getAttribute("style") === "display:none;") {
                    lastEvent = lastEvent.previousElementSibling;
                }
                targetReached = lastEvent == targetEvent;
            }
        }
    }
});

function n_restartEvent(event) {
    n_network.setSelection([], {unselectAll: true});
    n_network.nodes = [];
    n_network.links = [];
    n_network.packages = [];
    
    n_performEvent(event);
    n_network.redraw();
    return event;
}

function n_performEvent(event) {
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
            n_addNode(eventargs);
        else if (eventtype === "ConnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            n_addEdge(from, to);
        }
        else if (eventtype === "DisconnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];

            n_removeEdge(from, to);
            n_removeEdge(to, from);
        } else if (eventtype === "UnicastMessage" || eventtype == "MulticastMessage") {
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

function n_revertEvent(event) {
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
            n_removeNode(eventargs);
        else if (eventtype === "ConnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            n_removeEdge(from, to);
            n_removeEdge(to, from);
        } else if (eventtype === "DisconnectPeer") {
            var from = eventargs.split(",")[0];
            var to = eventargs.split(",")[1];
            n_addEdge(from, to);
        } else if (eventtype === "UnicastMessage" || eventtype == "MulticastMessage") {
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

n_muObserver.observe(document.getElementById("ss_elem_list"), {attributes:true, subtree:true, attributeOldValue:true});

google.load("visualization", "1");

// Set callback to run when API is loaded
google.setOnLoadCallback(n_drawVisualization);
// Called when the Visualization API is loaded.
// Called when the Visualization API is loaded.
function n_drawVisualization() {
    /*n_nodes = new vis.DataSet([
        {id: '1', label: 'Node 1'},
        {id: '2', label: 'Node 2'},
        {id: '3', label: 'Node 3'},
        {id: '4', label: 'Node 4'},
        {id: '5', label: 'Node 5'}
    ]);

    n_edges = new vis.DataSet([
        {id: '1', from: '1', to: '2'},
        {id: '2', from: '1', to: '3'},
        {id: '3', from: '2', to: '4'},
        {id: '4', from: '2', to: '5'}
    ]);*/

    n_nodes = new vis.DataSet([]);
    n_edges = new vis.DataSet([]);
    var data = {
        nodes: n_nodes,
        edges: n_edges
    };

    var options = {
        width: '800px',
        height: '600px',
        configure: {
          enabled: true,
          filter:function (option, path) {
            if (path.indexOf('physics') !== -1) {
              return true;
            }
            if (path.indexOf('smooth') !== -1 || option === 'smooth') {
              return true;
            }
            return false;
          },
          container: document.getElementById("configure-list"),
          showButton: true
        }
    };

    // Instantiate our graph object.
    n_network = new vis.Network(document.getElementById('new-eventvisualize'), data, options);
    
    var frame = document.getElementById('new-eventvisualize');
    frame.rsz = document.createElement("div");
    frame.rsz.id = "resizer";
    frame.rsz.style.position = "absolute";
    frame.rsz.style.right = '0';
    frame.rsz.style.bottom = '0';
    frame.rsz.style.width = '0';
    frame.rsz.style.height = '0';
    frame.rsz.style.borderStyle = "solid";
    frame.rsz.style.borderWidth = "0 0 20px 20px";
    frame.rsz.style.borderColor = "transparent transparent #ff6600 transparent";
    frame.rsz.style.cursor = "pointer";
    frame.rsz.style.zIndex = "10";
    frame.appendChild(frame.rsz);
    var doc = document,
        main = frame,
        ht, wd,
        x, y, dx, dy;
    
    var startResize = function(evt) {
        x = evt.screenX;
        y = evt.screenY;
        ht = parseInt(main.clientHeight);
        wd = parseInt(main.clientWidth);
    };
    
    var resize = function(evt) {
        dx = evt.screenX - x;
        dy = evt.screenY - y;
        x = evt.screenX;
        y = evt.screenY;
        wd += dx;
        ht += dy;
        //main.style.width = wd + "px";
        //main.style.height = ht + "px";
        n_network.setSize( wd + "px", ht + "px");
        n_network.redraw();
    };
    frame.rsz.addEventListener("mousedown", function(evt) {
        startResize(evt);
        doc.body.addEventListener("mousemove", resize);
        doc.body.addEventListener("mouseup", function() {
            doc.body.removeEventListener("mousemove", resize);
        });
    });
    // Add event listeners for node selection
    google.visualization.events.addListener(n_network, 'select', onNodeSelect);
}

function n_resetVisualization() {
    // Reset network event visualization
    n_network.setSelection([], {unselectAll: true});
    n_network.nodes = [];
    n_network.links = [];
    n_network.packages = [];

    // Reset ledger event visualization
    var nodesArray = [
        {id:"0", label:"Base node"}
    ];
    var edgesArray = [
    ];
    n_nodes = new vis.DataSet(nodesArray);
    n_edges = new vis.DataSet(edgesArray);
}

function n_addNode(nodeid) {
    try {
        n_nodes.add({id: nodeid, label: nodeid});
    }
    catch(err) {
        alert(err);
    }
}

function n_removeNode(nodeid) {
    try {
        n_nodes.remove({id: nodeid})
    }
    catch(err) {
        alert(err);
    }
};

function n_addEdge(from, to) {
    try {
        n_edges.add({id: from+to, from: from, to: to});
    }
    catch(err) {
        alert(err);
    }
}

function n_removeEdge(from, to) {
    try {
        n_edges.remove({id: from+to});
    }
    catch(err) {
        alert(err);
    }
}