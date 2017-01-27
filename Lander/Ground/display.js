var graphVec = function (ctx, vec) {
  return new Rickshaw.Graph({
        element: ctx,
        renderer: 'line',
        series: [{
                data: vec.x,
                color: 'steelblue'
        }, {
                data: vec.y,
                color: 'lightblue'
        }, {
                data: vec.z,
                color: 'darkblue'
        }]
      })
};

var graphData = function (ctx, data) {
  return new Rickshaw.Graph({
        element: ctx,
        renderer: 'line',
        series: [{
                data: data,
                color: 'steelblue'
              }]
            })
};

//graph = graphData(elem, datas)
//graph.render()
//graph.update()
