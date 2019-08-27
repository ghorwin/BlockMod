# GraphicsView Konzept und Umsetzungsstrategie

## ConnectorItem - Moving

Problem: QGraphicsItem::itemChange() post-event wird verwendet.

Algorithmus:
- Verschiebung (dx, dy) wird Quellseitig (Richtung Startknoten) und Zielseitig (zum Endknoten) aufgeprägt
- Es wird nach einem Segment mit passender Ausrichtung gesucht, und dieses dann entsprechend verlängert/verkürzt.

### Beispiel 1: 

Segment 2 (Horizontal) wird um dx,dy verschoben.

1. Quellseitige Anpassung:

- Segment 1 ist Vertikal -> seg1.offset += dy  -> dy = 0
- Segment 0 ist Horizontal -> seg0.offset += dx -> dx = 0

2. Zielseitige Anpassung:

- Segment 3 ist Vertikal -> seg3.offset -= dy -> dy = 0
- keine weiteren Segmente, Es verbleibt ein _dx_. 

Es wird das Segment 4 hinzugefügt, Ausrichtung Horizontal, Länge -dx.

Die Gesamtlänge der Verbindung bleibt unverändert (+dx -dx, +dy -dy).


### Beispiel 2:

Segment 0 (Ausrichtung Horizontal) wird horizontal verschoben.

1. Quellseitige Anpassung:

- keine weiteren Segmente
- es verbleibt dx -> Längendehnung des aktuellen Segments (seg0.offset += dx)

2. Zielseitige Anpassung:

- keine weiteren Segmente
- es verbleibt dx -> Längendehnung des aktuellen Segments (seg0.offset -= dx)


## Anpassung der Liniensegmente

- Liniensegment-Item hat Zugriff auf Connector, und hält den eigenen Index im Vektor der Segmente


### Problem: Änderung des Liniensegments (setLine) innerhalb von itemChange()

-> setLine() setzt Koordinaten relativ zur Item-Position. Diese ist zu diesem Zeitpunkt noch nicht aktualisiert. D.h. wenn, z.b. eine Verschiebung von (8,0) auf die Linie (0,0 -> 0,20) aufgeprägt wird, und mit setLine(QPointF(8,0),QPointF(8,20)) gesetzt wird,
dann wird die Linie beim nächsten Zeichnen an die Szenen-Koordinaten (16,0)...(16,20) gezeichnet, da die Position (8,0) *nach* Ende der itemChange() Funktion gesetzt wird.

Workaround:

-> neue Linie berechnen (globale Koordinaten -> umrechnen auf lokale Koordinaten der *aktuellen* Linienposition mittels line.translate(- item->pos() )
-> noch vor Ende der Funktion, diese Koordinaten noch um den Verschiebungsoffset (zukünftige pos-Verschiebung) korrigieren, line.translate(- moveDist )
-> und Linienpunkte mit setLine() setzen  (würde die Linie jetzt gezeichnet, so würde sie um moveDist nach links-oben verschoben sein, erst durch Anpassung der Linienposition stimmen die Koordinaten wieder


### Problem: Hinzufügen neuer Liniensegmente und Abgleich zwischen Segment-Vektor im Connector-Objekt und Linienobjekten

Anforderung:
- das aktuell bewegte Objekt muss gleich bleiben und den gleichen Segmentindex behalten
- alle anderen Linienobjekte können weggeworfen und neu erstellt werden, oder einfach durch Linienkoordinaten-Setzen aktualisiert werden

Algorithmus:
- aktuelles Liniensegment wird identifiziert durch Zeiger auf LiniensegmentItem und Segmentindex, letzterer bereits erhöht wenn links davon ein neues Segment hinzugefügt wurde
- erstelle Vektor aller existierenden Liniensegmentitems dieses Connectors, ausgenommen aktuelles Liniensegment
- passe Anzahl an (entferne/füge Liniensegmente aus Vektor hinzu, bis passend zum Segmente Vektor)
- sortiere Liniensegmentitem-Zeige an richtigen Index ein

Schleife über alle Liniensegmente -> Linienkoordinaten aktualisieren





