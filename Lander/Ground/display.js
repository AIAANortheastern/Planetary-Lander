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

    //d3.select(id).append("text").text("sdlfjsldgkfsdjfksldkfjldkjsfg");
}

//if you want to send data about direction
//buttons would need to be added
//dataSocket.send("LEFT/RIGHT");

//initial data needs to be sent in data format:  
// data = {"panorama": "...IMAGE FILE...", "image": "...IMAGE FILE...", "Packet Count" : ,
// "Temperature": {x: ..., y: ...}, "Humidity": {x: ..., y: ...}, 
// "Light": {x: ..., y: ...}, "Voltage": {x: ..., y: ...}, "Lattitude": {x: ..., y: ...},
// "Longitude": {x: ..., y: ...}, "Ax": {x: ..., y: ...}, "Ay": {x: ..., y: ...},
// "Az": {x: ..., y: ...}, "Gx": {x: ..., y: ...}, "Gy": {x: ..., y: ...},
// "Gz": {x: ..., y: ...}, "Mx": {x: ..., y: ...}, "My": {x: ..., y: ...}, "Mz": {x: ..., y: ...}
// } 
// x is time in seconds


dataSocket.onopen = function (event) {
};

dataSocket.onmessage = function(event) {
    var data = JSON.parse(event.data);

    // if the graphs haven't been rendered yet: 
    if(graphs.length == 0){
        // add first data point to array
        if("Temperature" in event.data)
            seriesData[0][0] = data.Temperature;
        if("Humidity" in event.data)
            seriesData[1][0] = data.Humidity;
        if("Light" in event.data)
            seriesData[2][0] = data.Light;
        if("Voltage" in event.data)
            seriesData[3][0] = data.Voltage;
        if("Lattitude" in event.data)
            seriesData[4][0] = data.Lattitude;
        if("Longitude" in event.data)
            seriesData[5][0] = data.Longitude;
        if("Ax" in event.data)
            seriesData[6][0] = data.Ax;
        if("Ay" in event.data)
            seriesData[7][0] = data.Ay;
        if("Az" in event.data)
            seriesData[8][0] = data.Az;
        if("Gx" in event.data)
            seriesData[9][0] = data.Gx;
        if("Gy" in event.data)
            seriesData[10][0] = data.Gy;
        if("Gz" in event.data)
            seriesData[11][0] = data.Gz;
        if("Mx" in event.data)
        seriesData[12][0] = data.Mx;
        if("My" in event.data)
            seriesData[13][0] = data.My;
        if("Mz" in event.data)
            seriesData[14][0] = data.Mz;

        // initialize graphs
        for(int i=0; i<ids.length; i++)
        {
            renderGraph(graphs[i], xAxes[i], yAxes[i], seriesdata[i]);
        }
    }
    else {
        if("Temperature" in event.data)
            seriesData[0].push(data.Temperature);
        if("Humidity" in event.data)
            seriesData[1].push(data.Humidity);
        if("Light" in event.data)
            seriesData[2].push(data.Light);
        if("Voltage" in event.data)
            seriesData[3].push(data.Voltage);
        if("Lattitude" in event.data)
            seriesData[4].push(data.Lattitude);
        if("Longitude" in event.data)
            seriesData[5].push(data.Longitude);
        if("Ax" in event.data)
            seriesData[6].push(data.Ax);
        if("Ay" in event.data)
            seriesData[7].push(data.Ay);
        if("Az" in event.data)
            seriesData[8].push(data.Az);
        if("Gx" in event.data)
            seriesData[9].push(data.Gx);
        if("Gy" in event.data)
            seriesData[10].push(data.Gy);
        if("Gz" in event.data)
            seriesData[11].push(data.Gz);
        if("Mx" in event.data)
            seriesData[12].push(data.Mx);
        if("My" in event.data)
            seriesData[13].push(data.My);
        if("Mz" in event.data)
            seriesData[14].push(data.Mz);

        for(var i=0; i<ids.length; i++){
            graphs[i].update();
        }
    }

    document.getElementById("packetsreceived").textContent = "Packet Count=" + event.data['Packet Count'];
    //document.getElementById("json-image").src = event.data.image;
    //document.getElementById("panorama").src = event.data.panorama;
}


/*
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
    for(var i=0; i<9; i++){
        graphs[i].update();
    }
}, 1000 );

document.getElementById("packetsreceived").textContent = 55;
*/
