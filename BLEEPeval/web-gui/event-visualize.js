var network = null;
var physics = true;
var nodes, edges, packages;
var nodesBuffer = [],
    edgesBuffer = [],
    packagesBuffer = [],
    blocksBuffer = [];
var bufferedOperation = '';

var ledgerNodes, ledgerEdges;
var ledger = null;

var mapNodes, mapEdges;
var blockchainMap = null;

var ImgDIR = 'link_network/img/';

var toggleButton = document.getElementById("toggle-btn");
toggleButton.addEventListener('click', toggle);

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
        applyBuffers();
        onNodeSelect(); // update event list for selected node
    }
});

function restartEvent(event) {
    network.destroy();
    document.getElementById('configure-list').innerHTML = '';
    drawVisualization();

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

        switch (eventtype) {
            case "InitPeerId":
                addNode(eventargs);
                break;
            case "ConnectPeer":
                var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                addEdge(from, to);
                break;
            case "DisconnectPeer":
                // Ignore disconnection because it is not reversible yet

                /* var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                removeEdge(from, to);
                removeEdge(to, from);*/
                alert("Disconnect is not supported yet");
                break;
            case "UnicastMessage":  // fallthrough
            case "MulticastingMessage":
                var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                var hashId = eventargs.split(",")[3];
                sendMessage(from, to, hashId);
                break
            case "RecvMessage":
                var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                var hashId = eventargs.split(",")[3];
                recvMessage(from, to, hashId);
                break;
            case "BlockAppend":
                var peerId = eventhost;
                var hash = eventargs.split(",")[1];
                var prevHash = eventargs.split(",")[2];
                var timestamp = eventargs.split(",")[3];
                appendBlock(peerId, hash, prevHash, timestamp);
                break;
        }
    }
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

        switch (eventtype) {
            case "InitPeerId":
                removeNode(eventargs);
                break;
            case "ConnectPeer":
                var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                removeEdge(from, to);
                break;
            case "DisconnectPeer":
                // Ignore disconnection because it is not reversible yet

                /*var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                addEdge(from, to);
                addEdge(to, from);*/
                alert("Disconnect is not supported yet");
                break;
            case "UnicastMessage":  // fallthrough
            case "MulticastingMessage":
                var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                var hashId = eventargs.split(",")[3];
                unsendMessage(from, to, hashId);
                break;
            case "RecvMessage":
                var from = eventargs.split(",")[0];
                var to = eventargs.split(",")[1];
                var hashId = eventargs.split(",")[3];
                unrecvMessage(from, to, hashId);
                break;
            case "BlockAppend":
                var peerId = eventhost;
                var hash = eventargs.split(",")[1];
                var prevHash = eventargs.split(",")[2];
                var timestamp = eventargs.split(",")[3];
                removeBlock(peerId, hash, prevHash, timestamp);
                break;
        }
    }
    return event;
}

muObserver.observe(document.getElementById("ss_elem_list"), {attributes:true, subtree:true, attributeOldValue:true});

google.load("visualization", "1");

// Set callback to run when API is loaded
google.setOnLoadCallback(drawVisualization);

// Called when the Visualization API is loaded
function drawVisualization() {
    nodesBuffer = [];
    edgesBuffer = [];
    packagesBuffer = [];
    bufferedOperation = '';

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
                if (path.indexOf('physics') !== -1) { return true}
                if (path.indexOf('smooth') !== -1 || option === 'smooth') { return true}
                if (path.indexOf('interaction') !== -1 && option.indexOf('hide') !== -1) { return true}
                return false;
            },
            'container': document.getElementById("configure-list"),
            'showButton': true
        },
        'nodes': {
            'shape': 'box'
        },
        'edges': {
            'color': {
                'color': "#cccccc",
                'highlight': "#343434"
            },
            "smooth": false
        },
        'packages': {
            'image': ImgDIR + 'transaction_32.png',
            'shape': 'image'
        },
        'interaction': {
            'hideEdgesOnDrag': true,
            'multiselect': true
        },
        "physics": {
            "forceAtlas2Based": {
                "springLength": 200,
                "springConstant": 0.03,
                "avoidOverlap": 0.24
            },
            "minVelocity": 0.75,
            "solver": "forceAtlas2Based",
            "timestep": 0.23
        }
    };

    // Instantiate our graph object.
    network = new vis.Network(document.getElementById('eventvisualize'), data, options);

    drawResizer('eventvisualize', network);

    // Stops network physics after initial loading
    network.on('stabilized', function() {
        physics = false;
        network.setOptions({'physics': {'enabled': false}});
        toggleButton.setAttribute('toggled', '');
        network.off('stabilized');
    });

    // Add event listeners for node selection
    network.on('selectNode', onNodeSelect);

    // Draw ledger event visualization
    var container = document.getElementById('ledgereventvisualize');

    var nodesArray = [
        {id:"0000000000", label:"genesis"}
    ];
    var edgesArray = [
    ];
    ledgerNodes = new vis.DataSet(nodesArray);
    ledgerEdges = new vis.DataSet(edgesArray);
    var data = {
        nodes: ledgerNodes,
        edges: ledgerEdges
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

    container = document.getElementById('blockchain-map');
    var nodesArray = [
        {id:"0000000000", label:"genesis"}
    ];
    var edgesArray = [
    ];
    mapNodes = new vis.DataSet(nodesArray);
    mapEdges = new vis.DataSet(edgesArray);
    var data = {
        nodes: mapNodes,
        edges: mapEdges
    };
    var mapplotoptions = {
        height: '300px',
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
    blockchainMap = new vis.Network(container, data, mapplotoptions);
    blockchainMap.on('selectNode', loadSnapshot);
}

function recvMessage(from, to, hashId, buffered = true) {
    if (buffered == true) {
        pushToBuffer(packagesBuffer, {id: from+to+hashId}, 'remove');
    }
    else {
        try {
            packages.remove({id: from+to+hashId});
        }
        catch(err) {
            alert("Message " + hashId + " " + err);
        }
    }
}

function unrecvMessage(from, to, hashId, buffered = true) {
    sendMessage(from, to, hashId, buffered);
}

function sendMessage(from, to, hashId, buffered = true) {
    if (buffered == true) {
        let edgeId = findEdgeId(from, to);
        pushToBuffer(packagesBuffer,
            {id: from+to+hashId, edge: edgeId, from: from, to: to},
            'add'
        );
    }
    else {
        try {
            packages.add({id: from+to+hashId, edge: from+to});
        }
        catch(err) {
            alert("Message " + hashId + " " + err);
        }
    }
}

function findEdgeId(from, to) {
    let id = [from+to, to+from].find( function(edgeId) {

        let edgeFound = edges.get(edgeId) || undefined;
        if (edgeFound !== undefined) {
            // The edge exists but its deletion might be buffered
            if (bufferedOperation != 'remove') {
                return true;
            }
            else {
                let removedEdge = edgesBuffer.find((item) => item.id == edgeId);
                if (removedEdge === undefined) {
                    return true;
                }
            }
        }

        // The edge doesn't exist but its creation might be buffered
        else if (bufferedOperation == 'add') {
            let edge = edgesBuffer.find((edge) => edge.id == edgeId);
            if (edge !== undefined) {
                return true;
            }
        }
    });
    return id;
}

function unsendMessage(from, to, hashId, buffered = true) {
    if (buffered == true) {
        pushToBuffer(packagesBuffer,
            {id: from+to+hashId, progress: {autoProgress: false}},
            'remove'
        );
    }
    else {
        try {
            packages.update({id: from+to+hashId, progress: {autoProgress: false}})
            packages.remove({id: from+to+hashId});
        }
        catch(err) {
            alert("Message " + hashId + " " + err);
        }
    }
}

function addNode(nodeid, buffered = true) {
    if (buffered == true) {
        pushToBuffer(nodesBuffer, {id: nodeid, label: nodeid}, 'add');
    }
    else {
        try {
            nodes.add({id: nodeid, label: nodeid});
        }
        catch(err) {
            alert(err);
        }
    }
}

function removeNode(nodeid, buffered = true) {
    if (buffered == true) {
        pushToBuffer(nodesBuffer, {id: nodeid}, 'remove')
    }
    else {
        try {
            nodes.remove({id: nodeid})
        }
        catch(err) {
            alert(err);
        }
    }
};

function addEdge(from, to, buffered = true) {
    let edge;
    if (from.startsWith("client")) {
        edge = {id: from+to, from: from, to: to, arrows: "middle",
            color:{color:"red", highlight: "red"}};
    }
    else {
        edge = {id: from+to, from: from, to: to, arrows: "middle"};
    }

    if (buffered == true) {
            pushToBuffer(edgesBuffer, edge, 'add');
    }
    else {
        try {
            edges.add(edge);
        }
        catch(err) {
            alert(err);
        }
    }
}

function removeEdge(from, to, buffered = true) {
    if (buffered == true) {
        pushToBuffer(edgesBuffer, {id: from+to}, 'remove');
    }
    else {
        try {
            edges.remove({id: from+to});
        }
        catch(err) {
            alert(err);
        }
    }
}

function appendBlockMap(peerId, hash, prevHash, timestamp) {
    if (prevHash === "0000000000") {
        // (catch err because dataSet returns error when we try to add existed node)
        try {
            mapNodes.add({id:prevHash, label:"genesis"});
        } catch(err) {

        }
    }

    try {
        mapNodes.add({id:hash, label:timestamp});
    } catch(err) {
    }

    try {
        mapEdges.add({id:prevHash+hash, from:prevHash, to:hash});
    } catch(err) {
    }

    mapEdges.remove(peerId);
    try {
        mapEdges.add({id:peerId, from:hash, to:peerId});
    } catch(err) {
    }
    try {
        mapNodes.update({id:peerId, label:peerId});
    } catch(err) {}
}

function appendBlock(peerId, hash, prevHash, timestamp) {
    if (prevHash === "0000000000") {
        // (catch err because dataSet returns error when we try to add existed node)
        try {
            ledgerNodes.add({id:prevHash, label:"genesis"});
        } catch(err) {

        }
    }

    try {
        ledgerNodes.add({id:hash, label:timestamp});
    } catch(err) {
    }

    try {
        ledgerNodes.add({id:prevHash, label:timestamp});
    } catch(err) {
    }


    try {
        ledgerEdges.add({id:prevHash+hash, from:prevHash, to:hash});
    } catch(err) {
    }

    ledgerEdges.remove(peerId);
    try {
        ledgerEdges.add({id:peerId, from:hash, to:peerId});
    } catch(err) {
    }
    try {
        ledgerNodes.update({id:peerId, label:peerId});
    } catch(err) {}

    // ledgerNodes.remove({id:peerId});
    // ledgerEdges.update({id:peerId, from:hash, to:peerId});
    // ledgerNodes.add({id:peerId, label:peerId});

}

function removeBlock(peerId, hash, prevHash, timestamp) {

    ledgerEdges.remove(peerId);
    try {
        ledgerEdges.add({id:peerId, from:prevHash, to:peerId});
    } catch(err) {
    }
    try {
        ledgerNodes.update({id:peerId, label:peerId});
    } catch(err) {}

    if ( ledgerEdges.distinct("from").find(from => from == hash) == undefined ) {
        // If no edge start from node "hash", delete node "hash"
        ledgerNodes.remove(hash);
        ledgerEdges.remove(prevHash+hash);

        if ( ledgerEdges.get({filter: edge => edge.to == prevHash}).length == 0
        && ledgerEdges.get({filter: edge => edge.from == prevHash}).length == 1 ) {
            // If prevHash is the first block and has no child
            ledgerNodes.remove([prevHash, peerId]);
        }
    }

}

function pushToBuffer(buffer, data, operation) {
    if (bufferedOperation != operation) {
        applyBuffers();
        bufferedOperation = operation;
    }
    buffer.push(data);
}

function applyBuffers() {
    if (bufferedOperation == 'add') {
        applyBuffer(nodes, nodesBuffer, 'add');
        applyBuffer(edges, edgesBuffer, 'add');
        applyBuffer(packages, packagesBuffer, 'add');
    }
    else if (bufferedOperation == 'remove') {
        // When unsending messages, this clears the autoProgress option
        let buff = Array.from(packagesBuffer);
        applyBuffer(packages, packagesBuffer, 'update');

        applyBuffer(packages, buff, 'remove');
        applyBuffer(edges, edgesBuffer, 'remove');
        applyBuffer(nodes, nodesBuffer, 'remove');
    }
    bufferedOperation = '';
}
function applyBuffer(dataset, buffer, operation) {
    try {
        if (operation == 'add') {
            dataset.add(buffer);
        }
        else if (operation == 'remove') {
            dataset.remove(buffer);
        }
        else {
            dataset.update(buffer);
        }
        buffer.length = 0;  // clears every existing reference of "buffer"
    }
    catch(err) {
        alert(err);
    }
}

// Called after receiving the eventlog
function endInitialLoading() {
    applyBuffers();

    var lastItem = document.getElementById("ss_elem_list").lastChild;
    while (lastItem && lastItem.getAttribute("style") === "display:none;") {
        lastItem = lastItem.previousElementSibling;
    }
    if (lastItem && lastItem.getAttribute("style") !== "display:none;") {
        lastItem.click();
    }
}

// Toggles the animation of the network based on the physics of nodes and links
function toggle() {
    physics = !physics;
    if (physics) {
        toggleButton.removeAttribute('toggled');
    }
    else {
        toggleButton.setAttribute('toggled', '');
    }
    network.setOptions({'physics': {'enabled': physics}});
}

function onNodeSelect() {
    var selection = network.getSelection();
    var nodeid;
    if (selection.nodes.length > 0) {
        nodeid = selection.nodes[selection.nodes.length - 1];
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

function loadSnapshot() {
    var selection = blockchainMap.getSelectedNodes();
    var blockId = selection[0];
    $('#ss_elem_list li[role="option"]:not([style="display:none;"]):contains(' + blockId + ')')[0].click();
}

// Custom resize button, we need this because vis Network captures click events on the canvas
function drawResizer(element, network) {
    var frame = document.getElementById(element);
    frame.rsz = document.createElement("div");
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
        ht, wd,
        x, y, dx, dy;
    
    var startResize = function(evt) {
        x = evt.screenX;
        y = evt.screenY;
        ht = parseInt(frame.clientHeight);
        wd = parseInt(frame.clientWidth);
    };
    
    var resize = function(evt) {
        dx = evt.screenX - x;
        dy = evt.screenY - y;
        x = evt.screenX;
        y = evt.screenY;
        wd += dx;
        ht += dy;
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
}
