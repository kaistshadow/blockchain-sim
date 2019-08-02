let util = require('../../util')
let DataSet = require('../../DataSet')
let DataView = require('../../DataView')
var Package = require('./components/Package').default

/**
 * Handler for Packages
 */

class PackagesHandler {
  /**
   * @param {Object} body
   * @param {Images} images
   * @param {Array.<Group>} groups
   * @param {LayoutEngine} layoutEngine
   */
  constructor(body, images) {
    this.body = body;
    this.images = images;

    // create the edge API in the body container
    this.body.functions.createPackage = this.create.bind(this);
    
    this.packagesListeners = {
      add: function add(event, params) {
        this.add(params.items);
      },
      update: function update(event, params) {
        this.update(params.items);
      },
      remove: function remove(event, params) {
        this.remove(params.items);
      }
    };

    this.options = {};
    this.defaultOptions = {
      borderWidth: 1,
      borderWidthSelected: 2,
      brokenImage: undefined,
      color: {
        border: '#2B7CE9',
        background: '#97C2FC',
        highlight: {
          border: '#2B7CE9',
          background: '#D2E5FF'
        },
        hover: {
          border: '#2B7CE9',
          background: '#D2E5FF'
        }
      },
      fixed: {
        x: false,
        y: false
      },
      font: {
        color: '#343434',
        size: 14, // px
        face: 'arial',
        background: 'none',
        strokeWidth: 0, // px
        strokeColor: '#ffffff',
        align: 'center',
        vadjust: 0,
        multi: false,
        bold: {
          mod: 'bold'
        },
        boldital: {
          mod: 'bold italic'
        },
        ital: {
          mod: 'italic'
        },
        mono: {
          mod: '',
          size: 15, // px
          face: 'monospace',
          vadjust: 2
        }
      },
      hidden: false,
      icon: {
        face: 'FontAwesome', //'FontAwesome',
        code: undefined, //'\uf007',
        size: 50, //50,
        color: '#2B7CE9' //'#aa00ff'
      },
      image: 'link_network/img/transaction_32.png', // --> URL
      label: 'label test',
      labelHighlightBold: true,
      level: undefined,
      margin: {
        top: 5,
        right: 5,
        bottom: 5,
        left: 5
      },
      radiusMin: 5,
      radiusMax: 10,
      scaling: {
        min: 10,
        max: 30,
        label: {
          enabled: false,
          min: 14,
          max: 30,
          maxVisible: 30,
          drawThreshold: 5
        },
        customScalingFunction: function customScalingFunction(min, max, total, value) {
          if (max === min) {
            return 0.5;
          } else {
            var scale = 1 / (max - min);
            return Math.max(0, (value - min) * scale);
          }
        }
      },
      shadow: {
        enabled: false,
        color: 'rgba(0,0,0,0.5)',
        size: 10,
        x: 5,
        y: 5
      },
      shape: 'square',
      shapeProperties: {
        borderDashes: false, // only for borders
        borderRadius: 6, // only for box shape
        interpolation: true, // only for image and circularImage shapes
        useImageSize: false, // only for image and circularImage shapes
        useBorderWithImage: false // only for image shape
      },
      size: 15,
      title: "coucou",
      value: undefined
    };

    util.deepExtend(this.options, this.defaultOptions);

    this.bindEventListeners();
  }

  /**
   * Binds event listeners
   */

  bindEventListeners() {
    var _this2 = this;

    // refresh the packages. Used when reverting from hierarchical layout
    this.body.emitter.on('refreshPackages', this.refresh.bind(this));
    this.body.emitter.on('refresh', this.refresh.bind(this));
    this.body.emitter.on('destroy', function () {
      util.forEach(_this2.packagesListeners, function (callback, event) {
        if (_this2.body.data.packages) _this2.body.data.packages.off(event, callback);
      });
      delete _this2.body.functions.createPackage;
      delete _this2.packagesListeners.add;
      delete _this2.packagesListeners.update;
      delete _this2.packagesListeners.remove;
      delete _this2.packagesListeners;
    });
  }

  /**
   *
   * @param {Object} options
   */

  setOptions(options) {
    if (options !== undefined) {
      Package.parseOptions(this.options, options);

      // update the shape in all packages
      if (options.shape !== undefined) {
        for (var packageId in this.body.packages) {
          if (this.body.packages.hasOwnProperty(packageId)) {
            this.body.packages[packageId].updateShape();
          }
        }
      }

      // update the font in all packages
      if (options.font !== undefined) {
        for (var _packageId in this.body.packages) {
          if (this.body.packages.hasOwnProperty(_packageId)) {
            this.body.packages[_packageId].updateLabelModule();
            this.body.packages[_packageId].needsRefresh();
          }
        }
      }

      // update the shape size in all packages
      if (options.size !== undefined) {
        for (var _packageId2 in this.body.packages) {
          if (this.body.packages.hasOwnProperty(_packageId2)) {
            this.body.packages[_packageId2].needsRefresh();
          }
        }
      }

      // update the state of the variables if needed
      if (options.hidden !== undefined) {
        this.body.emitter.emit('_dataChanged');
      }
    }
  }

  setData(packages) {
    var doNotEmit = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;

    var oldNodesData = this.body.data.packages;

    if (packages instanceof DataSet || packages instanceof DataView) {
      this.body.data.packages = packages;
    } else if (Array.isArray(packages)) {
      this.body.data.packages = new DataSet();
      this.body.data.packages.add(packages);
    } else if (!packages) {
      this.body.data.packages = new DataSet();
    } else {
      throw new TypeError('Array or DataSet expected');
    }

    if (oldNodesData) {
      // unsubscribe from old dataset
      util.forEach(this.nodesListeners, function (callback, event) {
        oldNodesData.off(event, callback);
      });
    }

    // remove drawn packages
    this.body.packages = {};

    if (this.body.data.packages) {
      // subscribe to new dataset
      var me = this;
      util.forEach(this.nodesListeners, function (callback, event) {
        me.body.data.packages.on(event, callback);
      });

      // draw all new packages
      var ids = this.body.data.packages.getIds();
      this.add(ids, true);
    }

    if (doNotEmit === false) {
      this.body.emitter.emit("_dataChanged");
    }
  }

  /**
   * Add packages
   * @param {number[] | string[]} ids
   * @param {boolean} [doNotEmit=false]
   * @private
   */

  add(ids) {
    var doNotEmit = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;

    var packages = this.body.packages;

    var newPackages = [];
    for (var i = 0; i < ids.length; i++) {
      id = ids[i];
      var properties = this.body.data.packages.get(id);
      var pkg = this.create(properties);
      newPackages.push(pkg);
      this.body.packages[id] = pkg; // note: this may replace an existing package
    }

    if (doNotEmit === false) {
      this.body.emitter.emit("_dataChanged");
    }
  }

  /**
   * Update existing packages, or create them when not yet existing
   * @param {number[] | string[]} ids id's of changed packages
   * @param {Array} changedData array with changed data
   * @param {Array|undefined} oldData optional; array with previous data
   * @private
   */

  update(ids, changedData, oldData) {
    var packages = this.body.packages;
    var dataChanged = false;
    for (var i = 0; i < ids.length; i++) {
      var id = ids[i];
      var pkg = packages[id];
      var data = changedData[i];
      if (pkg !== undefined) {
        // update package
        if (pkg.setOptions(data)) {
          dataChanged = true;
        }
      } else {
        dataChanged = true;
        // create package
        pkg = this.create(data);
        packages[id] = pkg;
      }
    }

    if (dataChanged === true) {
      this.body.emitter.emit("_dataChanged");
    } else {
      this.body.emitter.emit("_dataUpdated");
    }
  }

  /**
   * Remove existing packages. If packages do not exist, the method will just ignore it.
   * @param {number[] | string[]} ids
   * @private
   */

  remove(ids) {
    var packages = this.body.packages;

    for (var i = 0; i < ids.length; i++) {
      var id = ids[i];
      delete packages[id];
    }

    this.body.emitter.emit("_dataChanged");
  }

  /**
   * create a package
   * @param {Object} properties
   * @returns {Package}
   */

  create(properties) {
    return new Package(properties, this.body, this.images, this.options, this.defaultOptions);
  }

  /**
   * Refreshes Edge Handler
   */

  refresh() {
    var _this4 = this;

    util.forEach(this.body.edges, function (edge, edgeId) {
      var data = _this4.body.data.edges._data[edgeId];
      if (data !== undefined) {
        edge.setOptions(data);
      }
    });
  }
}

export default PackagesHandler
