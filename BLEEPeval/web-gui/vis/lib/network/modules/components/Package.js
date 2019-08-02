var util = require('../../../util')

var Label = require('./shared/Label').default
var ComponentUtil = require('./shared/ComponentUtil').default
var Box = require('./nodes/shapes/Box').default
var Circle = require('./nodes/shapes/Circle').default
var CircularImage = require('./nodes/shapes/CircularImage').default
var Database = require('./nodes/shapes/Database').default
var Diamond = require('./nodes/shapes/Diamond').default
var Dot = require('./nodes/shapes/Dot').default
var Ellipse = require('./nodes/shapes/Ellipse').default
var Icon = require('./nodes/shapes/Icon').default
var Image = require('./nodes/shapes/Image').default
var Square = require('./nodes/shapes/Square').default
var Hexagon = require('./nodes/shapes/Hexagon').default
var Star = require('./nodes/shapes/Star').default
var Text = require('./nodes/shapes/Text').default
var Triangle = require('./nodes/shapes/Triangle').default
var TriangleDown = require('./nodes/shapes/TriangleDown').default
var { printStyle } = require('../../../shared/Validator')

/**
 * A message sent along a visible edge
 */

class Package {
  /**
 * @param {Object} options        values specific to this edge, must contain at least 'from' and 'to'
 * @param {Object} body           shared state from Network instance
 * @param {Object} globalOptions  options from the EdgesHandler instance
 * @param {Object} defaultOptions default options from the EdgeHandler instance. Value and reference are constant
 */
  constructor(options, body, imagelist, globalOptions, defaultOptions) {

    if (body === undefined) {
      throw new Error("No body provided");
    }

    this.options = util.bridgeObject(globalOptions);
    this.globalOptions = globalOptions;
    this.defaultOptions = defaultOptions;
    this.body = body;

    // constants
    this.radiusMin = defaultOptions.radiusMin;
    this.radiusMax = defaultOptions.radiusMax;
    this.imagelist = imagelist;
    this.network = network;

    // initialize variables
    this.id = undefined;
    this.from = undefined;
    this.to = undefined;
    this.title = undefined; //useless, replaced by label in vis
    //this.shape = defaultOptions.shape;
    this.radius = defaultOptions.radius;
    this.color = defaultOptions.color;
    this.image = defaultOptions.image;
    this.value = undefined;
    this.progress =  0.0;
    this.timestamp = undefined;
    this.duration = defaultOptions.duration;
    this.autoProgress = true;
    this.radiusFixed = false;

    this.labelModule = new Label(this.body, this.options, false /* Not edge label */);
    // set properties
    this.setOptions(options);
  }

  /**
   * Set or overwrite options for the package
   * @param {Object} options  an object with options
   * @returns {null|boolean} null if no options, boolean if date changed
   */

  setOptions(options) {
    var currentShape = this.options.shape;
    if (!options) {
      return;
    }

    if (options.id !== undefined) {this.id = options.id;}
    if (options.from !== undefined) {this.fromId = options.from;}
    if (options.to !== undefined) {this.toId = options.to;}
    if (options.edge !== undefined) {this.edgeId = options.edge;}
    if (options.value !== undefined) {options.value = parseFloat(options.value);}
    if (options.radius != undefined) {this.radius = options.radius;}
    if (options.image != undefined) {this.image = options.image;}
    if (options.color != undefined) {this.color = options.color;}
    if (options.dashlength != undefined) {this.dashlength = options.dashlength;}
    if (options.dashgap != undefined) {this.dashgap = options.dashgap;}
    if (options.altdashlength != undefined) {this.altdashlength = options.altdashlength;}
    if (options.progress != undefined) {this.progress = options.progress;}
    if (options.timestamp != undefined) {this.timestamp = options.timestamp;}
    if (options.duration != undefined) {this.duration = options.duration;}

    this.radiusFixed = this.radiusFixed || (options.radius != undefined);
    this.autoProgress = (this.autoProgress == true) ? (options.progress == undefined) : false;

    if (this.shape == 'image') {
      this.radiusMin = defaultOptions.widthMin;
      this.radiusMax = defaultOptions.widthMax;
    }

    // handle progress
    if (this.progress < 0.0) {this.progress = 0.0;}
    if (this.progress > 1.0) {this.progress = 1.0;}

    // binds the edge and the two end nodes
    this.connect();

    // this transforms all shorthands into fully defined options
    Package.parseOptions(this.options, options, true, this.globalOptions);

    var pile = [options, this.options, this.defaultOptions];
    this.chooser = ComponentUtil.choosify('package', pile);
    
    this._load_images();
    this.updateLabelModule(options);
    this.updateShape(currentShape);

    this.connect();

    if (options.hidden !== undefined) {
      dataChanged = true;
    }

  }

  /**
   * Load the images from the options, for the nodes that need them.
   *
   * TODO: The imageObj members should be moved to CircularImageBase.
   *       It's the only place where they are required.
   *
   * @private
   */

  _load_images() {
    // Don't bother loading for packages without images
    if (this.options.shape !== 'circularImage' && this.options.shape !== 'image') {
      return;
    }

    if (this.options.image === undefined) {
      throw new Error("Option image must be defined for package type '" + this.options.shape + "'");
    }

    if (this.imagelist === undefined) {
      throw new Error("Internal Error: No images provided");
    }

    if (typeof this.options.image === 'string') {
      this.imageObj = this.imagelist.load(this.options.image, this.options.brokenImage, this.id);
    } else {
      if (this.options.image.unselected === undefined) {
        throw new Error("No unselected image provided");
      }

      this.imageObj = this.imagelist.load(this.options.image.unselected, this.options.brokenImage, this.id);

      if (this.options.image.selected !== undefined) {
        this.imageObjAlt = this.imagelist.load(this.options.image.selected, this.options.brokenImage, this.id);
      } else {
        this.imageObjAlt = undefined;
      }
    }
  }

  /**
   * update the options in the label module
   *
   * @param {Object} options
   */

  updateLabelModule(options) {
    var pile = [options, this.options, this.globalOptions, // Currently set global package options
    this.defaultOptions];

    this.labelModule.update(this.options, pile);

    if (this.labelModule.baseSize !== undefined) {
      this.baseFontSize = this.labelModule.baseSize;
    }
  }

  /**
   *
   * @param {string} currentShape
   */

  updateShape(currentShape) {
    if (currentShape === this.options.shape && this.shape) {
      this.shape.setOptions(this.options, this.imageObj, this.imageObjAlt);
    } else {
      // choose draw method depending on the shape
      switch (this.options.shape) {
        case 'box':
          this.shape = new Box(this.options, this.body, this.labelModule);
          break;
        case 'circle':
          this.shape = new Circle(this.options, this.body, this.labelModule);
          break;
        case 'circularImage':
          this.shape = new CircularImage(this.options, this.body, this.labelModule, this.imageObj, this.imageObjAlt);
          break;
        case 'database':
          this.shape = new Database(this.options, this.body, this.labelModule);
          break;
        case 'diamond':
          this.shape = new Diamond(this.options, this.body, this.labelModule);
          break;
        case 'dot':
          this.shape = new Dot(this.options, this.body, this.labelModule);
          break;
        case 'ellipse':
          this.shape = new Ellipse(this.options, this.body, this.labelModule);
          break;
        case 'icon':
          this.shape = new Icon(this.options, this.body, this.labelModule);
          break;
        case 'image':
          this.shape = new Image(this.options, this.body, this.labelModule, this.imageObj, this.imageObjAlt);
          break;
        case 'square':
          this.shape = new Square(this.options, this.body, this.labelModule);
          break;
        case 'hexagon':
          this.shape = new Hexagon(this.options, this.body, this.labelModule);
          break;
        case 'star':
          this.shape = new Star(this.options, this.body, this.labelModule);
          break;
        case 'text':
          this.shape = new Text(this.options, this.body, this.labelModule);
          break;
        case 'triangle':
          this.shape = new Triangle(this.options, this.body, this.labelModule);
          break;
        case 'triangleDown':
          this.shape = new TriangleDown(this.options, this.body, this.labelModule);
          break;
        default:
          this.shape = new Ellipse(this.options, this.body, this.labelModule);
          break;
      }
    }
    this.needsRefresh();
  }

  /**
   * Connect to the edge and end nodes of the package
   */

  connect() {
    this.from = this.body.nodes[this.fromId];
    this.to = this.body.nodes[this.toId];
    this.edge = this.body.edges[this.edgeId];

    if (!this.from) {
      throw "Node with id " + this.fromId + " not found";
    }
    if (!this.to) {
        throw "Node with id " + this.toId + " not found";
    }
    if (!this.edge) {
        throw "Edge with id " + this.edgeId + " not found";
    }
  }

  /**
   * Reset the calculated size of the package, forces it to recalculate its size
   */

  needsRefresh() {
    this.shape.refreshNeeded = true;
  }

  /**
   * This process all possible shorthands in the new options and makes sure that the parentOptions are fully defined.
   * Static so it can also be used by the handler.
   *
   * @param {Object} parentOptions
   * @param {Object} newOptions
   * @param {boolean} [allowDeletion=false]
   * @param {Object} [globalOptions={}]
   * @param {Object} [groupList]
   * @static
   */

  static parseOptions(parentOptions, newOptions) {
    var allowDeletion = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : false;
    var globalOptions = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : {};
    var groupList = arguments[4];


    /*var fields = ['arrowStrikethrough', 'id', 'from', 'hidden', 'hoverWidth', 'labelHighlightBold', 
    'length', 'line', 'opacity', 'physics', 'scaling', 'selectionWidth', 'selfReferenceSize', 
    'to', 'title', 'value', 'width', 'font', 'chosen', 'widthConstraint'];*/
    var fields = ['color', 'shadow', 'size'];
    util.selectiveDeepExtend(fields, parentOptions, newOptions, allowDeletion);

    // merge the shadow options into the parent.
    util.mergeOptions(parentOptions, newOptions, 'shadow', globalOptions);

    // individual shape newOptions
    if (newOptions.color !== undefined && newOptions.color !== null) {
      var parsedColor = util.parseColor(newOptions.color);
      util.fillIfDefined(parentOptions.color, parsedColor);
    } else if (allowDeletion === true && newOptions.color === null) {
      parentOptions.color = util.bridgeObject(globalOptions.color); // set the object back to the global options
    }

    if (allowDeletion === true && newOptions.font === null) {
      parentOptions.font = util.bridgeObject(globalOptions.font); // set the object back to the global options
    }

    // handle the scaling options, specifically the label part
    if (newOptions.scaling !== undefined) {
      util.mergeOptions(parentOptions.scaling, newOptions.scaling, 'label', globalOptions.scaling);
    }
  }
}

export default Package
