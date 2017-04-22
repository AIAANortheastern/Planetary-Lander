//var dataSocket = new WebSocket("localhost");
var seriesData = [ [], [], [], [], [], [], [], [], [] ];
var graphs = [ ];
var xAxes = [ ];
var yAxes = [ ];
var ids = ["chart1", "chart2", "chart3", "chart4", "chart5", "chart6","chart7", "chart8", "chart9", "chart10", "chart11", "chart12", "chart13", "chart14","chart15"  /* ... */];
var palette = new Rickshaw.Color.Palette( { scheme: 'cool', interpolatedStopCount: 2 } );

function renderGraph(plotSet, id){
    var graph = new Rickshaw.Graph( {
            element: document.getElementById(id),
            renderer: 'line',
            series: [ {
                    color: palette.color(),
                    data: plotSet
            } ]
    } );
    graph.render();
    graphs.push(graph);
    xAxis = new Rickshaw.Graph.Axis.Time({
        graph: graph,
        timeFixture: new Rickshaw.Fixtures.Time.Local()
    });
    xAxis.render();
    xAxes.push(xAxis);
    yAxis = new Rickshaw.Graph.Axis.Y({
        graph: graph
    });
    yAxis.render();
    yAxes.push(yAxes);

    d3.select(id).append("text").text("sdlfjsldgkfsdjfksldkfjldkjsfg");
}

//if you want to send data about direction
//buttons would need to be added
//dataSocket.send("LEFT/RIGHT");

//initialize graphs with initial data
//initial data needs to be sent in data format: 
// data = {"panorama": "...IMAGE FILE...", "image": "...IMAGE FILE...", 
// "initialdata": [ [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is temperature
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is humidity
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is light intensity
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is voltage
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is gps lattitude
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is gps longitude
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is acceleration x
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is acceleration y
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is acceleration z
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is gyro x
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is gyro y
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is gyro z
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is mag x
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...], // y is mag y
// [ {x: ..., y: ...}, {x: ..., y: ...}, ...] ]} // y is mag z
// x is time in seconds

/*
dataSocket.onopen = function (event) {
    var data = JSON.parse(event.data);
    for(var i=0; i<Object.keys(data).length; i++){
        seriesData[i] = event.data.initialdata[i];
        renderGraph(graphs[i], xAxes[i], yAxes[i], seriesData[i]);
    }
    document.getElementById("json-image").src = event.data.image;
    document.getElementById("panorama").src = event.data.panorama;
};
*/

// update graphs with incoming data
// updated data needs to be send in data format: 
// data = { "image": "...IMAGE FILE...", 
// "updateddata": [ {x: ..., y: ...}, // y is temperature
// {x: ..., y: ...}, // y is humidity
// {x: ..., y: ...}, // y is light intensity
// {x: ..., y: ...}, // y is voltage
// {x: ..., y: ...}, // y is gps lattitude
// {x: ..., y: ...}, // y is gps longitude
// {x: ..., y: ...}, // y is acceleration x
// {x: ..., y: ...}, // y is acceleration y
// {x: ..., y: ...}, // y is acceleration z
// {x: ..., y: ...}, // y is gyro x
// {x: ..., y: ...}, // y is gyro y
// {x: ..., y: ...}, // y is gyro z
// {x: ..., y: ...}, // y is mag x
// {x: ..., y: ...}, // y is mag y
// {x: ..., y: ...} ]} // y is mag z
// x is time in seconds

/*
dataSocket.onmessage = function(event) {
    var newData = JSON.parse(event.data);
    // var time = new Date(msg.date);
    for(var i=0; i<Object.keys(data).length; i++){
        seriesData[i].push(event.data.updateddata[i]);
        graph[i].update();
    }
    document.getElementById("json-image").src = event.data.image;
};
*/

// FOR TESTING ONLY:
var random = new Rickshaw.Fixtures.RandomData(150);

for (var i = 0; i < 150; i++) {
    random.addData(seriesData);
}

console.log(seriesData);

for(var i=0; i<ids.length; i++){
    renderGraph(seriesData[1], ids[i]);
}

document.getElementById("json-image").src = "dog2.jpg";
setInterval( function() {
    random.addData(seriesData);
    for(var i=0; i<9/*graphs.length*/; i++){
        graphs[i].update();
    }
}, 1000 );
