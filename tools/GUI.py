import dash
from dash import html
import dash_leaflet as dl
from Decoder import Decoder

class GUI:
    def __init__(self, decoder: Decoder):
        self.newData: bool = True
        self.decoder: Decoder = decoder

        goodMarker = dict(
            iconUrl="/assets/icon_blue.png",
            iconSize=[18,23],
            iconAnchor=[18,12],
        )
        goodMarkers = [
            dl.Marker(
                position=pt, 
                children=dl.Tooltip(
                    content=self.decoder.waypointToText(pt)), 
                    icon=goodMarker
                ) 
            for pt in self.decoder.data
        ]

        badMarker = dict(
            iconUrl="/assets/icon_red.png",
            iconSize=[18,23],
            iconAnchor=[18,12],
        )
        badMarkers = [
            dl.Marker(
                position=pt, 
                children=dl.Tooltip(
                    content=self.decoder.waypointToText(pt)), 
                    icon=badMarker
                ) 
            for pt in self.decoder.getMissingPoints()
        ]

        markers = goodMarkers + badMarkers

        # Dash app
        self.app = dash.Dash(__name__)
        self.app.title = "Log Decoder"

        self.app.layout = html.Div([
            html.H2("Log Decoder", style={"textAlign": "center"}),
            dl.Map(
                id="map",
                center=self.decoder.getAverageWaypoint(),
                style={'width': '100%', 'height': '90vh'},
                children=[
                    dl.TileLayer(maxNativeZoom=25, maxZoom=25),
                    dl.LayerGroup(id="marker-layer", children=markers),
                    dl.Polyline(id="line", positions=[self.decoder.getWaypoints()], color="blue")
                ],
                zoom=17,
            ),
        ])
    