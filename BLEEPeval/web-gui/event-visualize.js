/**
 * Manage all the data for each vis.Network
 */
class NetworkData {
    constructor(data = {}) {
        this.setData(data);
    }

    setData(data) {
        this.nodes = new vis.DataSet(data.nodes);
        this.edges = new vis.DataSet(data.edges);
        this.packages = new vis.DataSet(data.packages);
        this.buffer = {
            nodes: [],
            edges: [],
            packages: {toAdd: {}, toRemove: {}},
            operation: ''
        }
        this.network = undefined;
    }

    /**
     * Used to buffer an modification of the data of a Network. This is much
     * faster than updating the Dataset every time.
     * 
     * @param {string} itemsType 
     * @param {Object} data 
     * @param {string} operation 
     */
    pushToBuffer(itemsType, data, operation) {
        // Packages history doesn't matter we can switch between 'add' and
        // 'remove' without applying the buffer to the Dataset
        if (itemsType == 'packages') {
            if (operation == 'add') {
                this._addPackage(data);
            }
            else {
                this._removePackage(data);
            }
        }
        else {
            // We can only buffer one operation at a time for both nodes or edges
            // because the order is not remembered when applying buffers. 
            if (this.buffer.operation != operation) {
                this.applyBuffers();
                this.buffer.operation = operation;
            }
            if (Array.isArray(data)) {
                for (var i = 0, len = data.length; i < len; i++) {
                    this.buffer[itemsType].push(data[i])
                }
            } else if (data && typeof data === 'object') {
                this.buffer[itemsType].push(data);
            }
        }
    }

    applyBuffers() {
        this._applyBuffer('nodes');
        this._applyBuffer('edges');
        this._applyPackagesBuffer();
        this.buffer.operation = '';
    }

    /**
     * Actually apply the content of a buffer to the corresponding Dataset
     * 
     * @param {string} itemsType 
     */
    _applyBuffer(itemsType) {
        try {
            if (this.buffer.operation == 'add') {
                this[itemsType].add(this.buffer[itemsType]);
            }
            else if (this.buffer.operation == 'remove') {
                this[itemsType].remove(this.buffer[itemsType]);
            }
            else {
                this[itemsType].update(this.buffer[itemsType]);
            }
            this.buffer[itemsType].length = 0;
        } catch(err) {}
    }

    /**
     * Push the addition of packages
     * 
     * @param {Object|Array} pkg    A package or array of packages
     */
    _addPackage(pkg) {
        let toAdd = this.buffer.packages.toAdd;
        let toRemove = this.buffer.packages.toRemove;
        if (Array.isArray(pkg)) {
            for (var i = 0, len = pkg.length; i < len; i++) {
                if (toRemove[pkg[i].id] != undefined)
                    delete toRemove[pkg[i].id];
                else {
                    toAdd[pkg.id] = pkg[i];
                }
            }
        } else if (pkg && typeof pkg === 'object') {
            if (toRemove[pkg.id] != undefined)
                delete toRemove[pkg.id];
            else {
                toAdd[pkg.id] = pkg;
            }
        }
    }

    /**
     * Push the deletion of packages
     * 
     * @param {Object|Array} pkg    A package or array of packages
     */
    _removePackage(pkg) {
        let toAdd = this.buffer.packages.toAdd;
        let toRemove = this.buffer.packages.toRemove;
        if (Array.isArray(pkg)) {
            for (var i = 0, len = pkg.length; i < len; i++) {
                if (toAdd[pkg[i].id] != undefined)
                    delete toAdd[pkg[i].id];
                else {
                    toRemove[pkg.id] = pkg[i];
                }
            }
        } else if (pkg && typeof pkg === 'object') {
            if (toAdd[pkg.id] != undefined)
                delete toAdd[pkg.id];
            else {
                toRemove[pkg.id] = pkg;
            }
        }
    }

    _applyPackagesBuffer() {
        this.packages.add(Object.values(this.buffer.packages.toAdd));
        this.packages.update(Object.values(this.buffer.packages.toRemove));
        this.packages.remove(Object.values(this.buffer.packages.toRemove));
        this.buffer.packages = {toAdd: {}, toRemove: {}};
    }
}

var physics = true; // physics of the event visualization network

// declare vis.Network objects
var network = null;    
var ledger = null;
var blockchainMap = null;

// data objects for each vis.Network
var visualizationData = undefined,
    ledgerData = undefined,
    mapData = undefined;

var ImgDIR = 'link_network/img/';

var peerHash = {};
var peerPrevHash = {};

// bind listeners
var toggleButton = document.getElementById("toggle-btn");
toggleButton.addEventListener('click', toggle);
document.addEventListener("keydown", function(event) {
    if (event.altKey === true && event.ctrlKey === true) {
        event.preventDefault();
        document.getElementById("ss_elem_list").focus({
            preventScroll: true
        });
    }
});

var muObserver = new MutationObserver(function(mutations) {

    var targetEvent;
    var lastEvent;
    var restart;
    var t0 = performance.now();

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
        var longJump = false;
        if (restart)
            restartEvent(targetEvent);

        else if (lastEventNumber < targetEventNumber) {
            while (!targetReached) {
                var nextEvent = lastEvent.nextElementSibling;
                while (nextEvent && nextEvent.getAttribute("style") === "display:none;") {
                    nextEvent = nextEvent.nextElementSibling;
                }
                if (nextEvent != targetEvent)
                    longJump = true;
                if (nextEvent)
                    lastEvent = performEvent(nextEvent, longJump);
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
        visualizationData.applyBuffers();
        ledgerData.applyBuffers();
        onNodeSelect(); // update event list for selected node
        // Even though the timeout is 0, this will be executed after redering the webport
        window.setTimeout(function() {
            console.log('elapsed time: ' + Math.floor(performance.now() - t0) + 'ms')
        });
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

function performEvent(event, buffered = true) {
    var eventlog = event.textContent;
    var rePattern = new RegExp(/(.+?),([0-9]+),(.+?),(.*)$/);
    var matches = eventlog.match(rePattern);
    if (matches) {
        var eventhost = matches[1];
        var eventtime = matches[2];
        var eventtype = matches[3];
        var eventargs = matches[4];
        //console.log("Performing " + eventtype);

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
                recvMessage(from, to, hashId, buffered);
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
        //console.log("Reverting " + eventtype);

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

    visualizationData = new NetworkData();

    peerHash = {};
    peerPrevHash = {};
    var data = {
        nodes: visualizationData.nodes,
        edges: visualizationData.edges,
        packages: visualizationData.packages
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
    ledgerData = new NetworkData({nodes: nodesArray, edges: edgesArray});
    var data = {
        nodes: ledgerData.nodes,
        edges: ledgerData.edges
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
        interaction: {
            dragNodes: false,
            hover: true,
            hoverConnectedEdges: false
        },
        physics: {
            enabled: false,
        },
        nodes : {
            shape: "box",
            size: 50
        },
        edges : {
            length: 1,
            chosen: false
        }
    };

    ledger = new vis.Network(container, data, ledgerplotoptions);

    if (blockchainMap == undefined) {
        container = document.getElementById('blockchain-map');
        var nodesArray = [
            {id:"0000000000", label:"genesis"}
        ];
        var edgesArray = [
        ];
        mapData = new NetworkData({nodes: nodesArray, edges: edgesArray});
        var data = {
            nodes: mapData.nodes,
            edges: mapData.edges
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
            interaction: {
                dragNodes :false,
                hover: true,
                hoverConnectedEdges: false
            },
            physics: {
                enabled: false
            },
            nodes : {
                shape: "box",
                size: 50
            },
            edges : {
                length: 1,
                chosen: false
            }
        };
        blockchainMap = new vis.Network(container, data, mapplotoptions);
        blockchainMap.on('selectNode', loadSnapshot);
    }
}

function recvMessage(from, to, hashId, buffered = true) {
    visualizationData.pushToBuffer(
        'packages',
        {id: from+to+hashId, progress: {autoProgress: !buffered}},
        'remove'
    );
}

function unrecvMessage(from, to, hashId) {
    sendMessage(from, to, hashId);
}

function sendMessage(from, to, hashId) {
    let edgeId = findEdgeId(from, to, visualizationData);
    visualizationData.pushToBuffer(
        'packages',
        {id: from+to+hashId, edge: edgeId, from: from, to: to},
        'add'
    );
}

function findEdgeId(from, to, data) {
    let id = [from+to, to+from].find( function(edgeId) {
        let bufferedEdges = data.buffer.edges;
        let edgeFound = data.edges.get(edgeId) || undefined;
        if (edgeFound !== undefined) {
            // The edge exists but its deletion might be buffered
            if (data.buffer.operation != 'remove') {
                return true;
            }
            else {
                let removedEdge = bufferedEdges.find((item) => item.id == edgeId);
                if (removedEdge === undefined) {
                    return true;
                }
            }
        }

        // The edge doesn't exist but its creation might be buffered
        else if (data.buffer.operation == 'add') {
            let edge = bufferedEdges.find((edge) => edge.id == edgeId);
            if (edge !== undefined) {
                return true;
            }
        }
    });
    return id;
}

function unsendMessage(from, to, hashId) {
    visualizationData.pushToBuffer(
        'packages',
        {id: from+to+hashId, progress: {autoProgress: false}},
        'remove'
    );
}

function addNode(nodeid) {
    visualizationData.pushToBuffer('nodes', {id: nodeid, label: nodeid}, 'add');
}

function removeNode(nodeid) {
    visualizationData.pushToBuffer('nodes', {id: nodeid}, 'remove');
};

function addEdge(from, to) {
    let edge;
    if (from.startsWith("client")) {
        edge = {id: from+to, from: from, to: to, arrows: "middle",
            color:{color:"red", highlight: "red"}};
    }
    else {
        edge = {id: from+to, from: from, to: to, arrows: "middle"};
    }
    visualizationData.pushToBuffer('edges', edge, 'add');
}

function removeEdge(from, to) {
    visualizationData.pushToBuffer('edges', {id: from+to}, 'remove');
}

function appendBlockMap(peerId, hash, prevHash, timestamp) {
    let newNodes = [
        {id: hash, label: timestamp, title: hash},
        {id: peerId, label: peerId}
    ];
    let newEdges = [
        {id: prevHash+hash, from: prevHash, to: hash},
        {id: peerId, from: hash, to: peerId}
    ];
    if (prevHash === "0000000000") {
        newNodes.unshift({id:prevHash, label:"genesis"});
    }
    mapData.pushToBuffer('nodes', newNodes, 'update');
    mapData.pushToBuffer('edges', newEdges, 'update');
}

function appendBlock(peerId, hash, prevHash, timestamp) {
    let newNodes = [
        {id: hash, label: timestamp, title: hash},
        {id: peerId, label: peerId}
    ];
    let newEdges = [
        {id: prevHash+hash, from: prevHash, to: hash},
        {id: peerId, from: hash, to: peerId}
    ];
    if (prevHash === "0000000000") {
        newNodes.unshift({id:prevHash, label:"genesis"});
    }
    ledgerData.pushToBuffer('nodes', newNodes, "update");
    ledgerData.pushToBuffer('edges', newEdges, "update");

    // add node pointer
    if (peerPrevHash[peerId] == undefined){
        peerPrevHash[peerId] = [];
    }
    peerPrevHash[peerId].push(peerHash[peerId]);
    peerHash[peerId] = hash;
}

function drawBLEEPNode() {
    for (var peer in peerHash) {
        try {
            visualizationData.edges.add({id:peer, from:peerHash[peer], to:peer});
        } catch(err) {}
        try {
            visualizationData.nodes.update({id:peer, label:peer});
        } catch(err) {}
    }
}

function removeBlock(peerId, hash, prevHash, timestamp) {
    peerHash[peerId] = peerPrevHash[peerId].pop();
    if (peerHash[peerId] != undefined) {
        ledgerData.pushToBuffer('edges', {id: peerId, from: peerHash[peerId]}, 'update');
    }
    else {
        ledgerData.pushToBuffer('edges', {id: peerId}, 'remove');
        ledgerData.pushToBuffer('nodes', {id: peerId}, 'remove');
    }
    let leafBlock = true;
    Object.values(peerHash).forEach(value => {
        if (value == hash) {
            leafBlock = false;
        }
    });
    if (leafBlock) {
        ledgerData.pushToBuffer('edges', {id: prevHash+hash}, 'remove');
        ledgerData.pushToBuffer('nodes', {id: hash}, 'remove');
    }
}

// Called after receiving the eventlog
function endInitialLoading() {
    mapData.applyBuffers();
    visualizationData.applyBuffers();
    ledgerData.applyBuffers();

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
        let item = items[i];

        if (item.innerHTML.includes(",API,") && item.innerHTML.startsWith(nodeid)) {
            let li = document.createElement("li");
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
    if (blockId == '0000000000') {
        document.getElementById("ss_elem_list")
            .querySelector('[role="option"]:not([style="display:none;"])')
            .click();
    }
    else {
        $('#ss_elem_list li[role="option"]:not([style="display:none;"]):contains(' + blockId + ')')[0].click();
    }
}

/**
 * Custom resize button, we need this because vis Network captures click events on the canvas
 * @param {string} element | The DOM element, may NOT have position static
 * @param {Object} network | The network created from this element
 */
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
