var nodesTable = null;
var linksTable = null;
var packa
var network = null;

var nodes, edges, packages;
var nodes_block, edges_block;
var ledger = null;
var ImgDIR = 'link_network/img/';


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

muObserver.observe(document.getElementById("ss_elem_list"), {attributes:true, subtree:true, attributeOldValue:true});

google.load("visualization", "1");

// Set callback to run when API is loaded
google.setOnLoadCallback(drawVisualization);

// Called when the Visualization API is loaded
function drawVisualization() {
    /*nodes = new vis.DataSet([
        {id: '1', label: 'Node 1'},
        {id: '2', label: 'Node 2'},
        {id: '3', label: 'Node 3'},
        {id: '4', label: 'Node 4'},
        {id: '5', label: 'Node 5'}
    ]);

    edges = new vis.DataSet([
        {id: '1', from: '1', to: '2'},
        {id: '2', from: '1', to: '3'},
        {id: '3', from: '2', to: '4'},
        {id: '4', from: '2', to: '5'}
    ]);*/

    nodes = new vis.DataSet([]);
    edges = new vis.DataSet([]);
    packages = new vis.DataSet([]);
    var data = {
        nodes: nodes,
        edges: edges,
        packages: packages
    };

    var options = {
        'width': '800px',
        'height': '600px',
        'configure': {
            'enabled': true,
            'filter':function (option, path) {
                if (path.indexOf('physics') !== -1) { return true;}
                if (path.indexOf('smooth') !== -1 || option === 'smooth') { return true;}
                return false;
            },
            'container': document.getElementById("configure-list"),
            'showButton': true
        },
        'edges': {
            'color': {
                'color': "#cccccc",
                'highlight': "#343434"
            },
            "smooth": false /*{
                "enabled": false,
                "type": "continuous",
                "forceDirection": "none",
                "roundness": 0
            }*/
        },
        'interaction': {
            'hideEdgesOnDrag': true,
            'multiselect': true
        },
        "physics": {
            "forceAtlas2Based": {
                "springLength": 100,
                "avoidOverlap": 0.24
            },
            "minVelocity": 0.75,
            "solver": "forceAtlas2Based",
            "timestep": 0.23
        }
    };

    // Instantiate our graph object.
    network = new vis.Network(document.getElementById('new-eventvisualize'), data, options);
    
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
        network.setSize( wd + "px", ht + "px");
        network.redraw();
    };
    frame.rsz.addEventListener("mousedown", function(evt) {
        startResize(evt);
        doc.body.addEventListener("mousemove", resize);
        doc.body.addEventListener("mouseup", function() {
            doc.body.removeEventListener("mousemove", resize);
        });
    });
    network.on('stablized', function (params) {
        network.setOptions({
            'physics': {'enabled': false}
        });
        console.log("stabilized, physics stopped!", params);
    });
    // Add event listeners for node selection
    //google.visualization.events.addListener(network, 'select', onNodeSelect);


    // Draw ledger event visualization
    var container = document.getElementById('ledgereventvisualize');

    var nodesArray = [
        {id:"0000000000", label:"genesis"}
    ];
    var edgesArray = [
    ];
    nodes_block = new vis.DataSet(nodesArray);
    edges_block = new vis.DataSet(edgesArray);
    var data = {
        nodes: nodes_block,
        edges: edges_block
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
    nodes = [];
    links = [];
    packages = [];

    // Reset ledger event visualization
    var nodesArray = [
        {id:"0", label:"Base node"}
    ];
    var edgesArray = [
    ];
    nodes_block = new vis.DataSet(nodesArray);
    edges_block = new vis.DataSet(edgesArray);
}

function recvMessage(from, to, hashId) {
    try {
        packages.remove({id: hashId, from: from, to: to});
    }
    catch(err) {
        alert(err);
    }
}

function unrecvMessage(from, to, hashId) {
    sendMessage(from, to, hashId);
}

function sendMessage(from, to, hashId) {
    try {
        packages.add({id: hashId, from: from, to: to});
    }
    catch(err) {
        alert(err);
    }
}

function unsendMessage(from, to, hashId) {
    recvMessage(from, to, hashId);
}

function addNode(nodeid) {
    try {
        nodes.add({id: nodeid, label: nodeid});
    }
    catch(err) {
        alert(err);
    }
}

function removeNode(nodeid) {
    try {
        nodes.remove({id: nodeid})
    }
    catch(err) {
        alert(err);
    }
};

function addEdge(from, to) {
    try {
        if (from.startsWith("client")) {
            edges.add({id: from+to, from: from, to: to, arrows: "middle", color:{color:"red", highlight: "red"}});
        }
        else
            edges.add({id: from+to, from: from, to: to, arrows: "middle"});
    }
    catch(err) {
        alert(err);
    }
}

function removeEdge(from, to) {
    try {
        edges.remove({id: from+to});
    }
    catch(err) {
        alert(err);
    }
}

function appendBlock(peerId, hash, prevHash, timestamp) {
    if (prevHash === "0000000000") {
        // (catch err because dataSet returns error when we try to add existed node)
        try {
            nodes_block.add({id:prevHash, label:"genesis"});
        } catch(err) {

        }
    }
    // (catch err because dataSet returns error when we try to add existed node)
    try {
        nodes_block.add({id:hash, label:timestamp});
    } catch(err) {
    }
    // (catch err because dataSet returns error when we try to add existed node)
    try {
        nodes_block.add({id:prevHash, label:timestamp});
    } catch(err) {
    }

    // (catch err because dataSet returns error when we try to add existed node)
    try {
        edges_block.add({id:prevHash+hash, from:prevHash, to:hash});
    } catch(err) {
    }

    // add node pointer
    try {

    } catch(err) {}


    edges_block.remove(peerId);
    try {
        edges_block.add({id:peerId, from:hash, to:peerId});
    } catch(err) {
    }
    try {
        nodes_block.update({id:peerId, label:peerId});
    } catch(err) {}

    // nodes_block.remove({id:peerId});
    // edges_block.update({id:peerId, from:hash, to:peerId});
    // nodes_block.add({id:peerId, label:peerId});


}

function removeBlock(peerId, hash, prevHash, timestamp) {

    edges_block.remove(peerId);
    try {
        edges_block.add({id:peerId, from:prevHash, to:peerId});
    } catch(err) {
    }
    try {
        nodes_block.update({id:peerId, label:peerId});
    } catch(err) {}

    if ( edges_block.distinct("from").find(from => from == hash) == undefined ) {
        //If no edge start from node "hash", delete node "hash"
        nodes_block.remove(hash);
        edges_block.remove(prevHash+hash);

        if ( edges_block.get({filter: edge => edge.to == prevHash}).length == 0
        && edges_block.get({filter: edge => edge.from == prevHash}).length == 1 ) {
            //If prevHash is the first block and has no child
            nodes_block.remove([prevHash, peerId]);
        }
    }

}

// Called after receiving the eventlog
function endInitialLoading() {
    var lastItem = document.getElementById("ss_elem_list").lastChild;
    while (lastItem && lastItem.getAttribute("style") === "display:none;") {
        lastItem = lastItem.previousElementSibling;
    }
    if (lastItem && lastItem.getAttribute("style") !== "display:none;") {
        lastItem.click();
    }
}

function onNodeSelect() {
    var sel = network.getSelection();
    var nodeid;
    if (sel.nodes.length > 0) {
        nodeid = network.node[0].id;
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
