#include "Dataset.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>

// Durch diese Funktion werden die zu analysierenden Datens�tze der Zoo-Besucher eingelesen und in dem �bergebenen Vektor f�r Zoo-Besucher abgelegt
void datenEinlesen(std::vector<Dataset*>& data, std::string csvPfad)
{
	std::ifstream csvDatei(csvPfad);
	std::string csvInhalt;

	// �berpr�fung ob die angegebene Datei erfolgreich gefunden und ge�ffnet werden kann.
	if (!csvDatei.good())
		return;

	// Die Titelzeile der CSV-Spalten wird eingelesen, aber nicht verwertet.
	std::getline(csvDatei, csvInhalt);

	/* Die ge�ffnete CSV-Datei wird zeilenweise eingelesen, solange das Datei-Ende noch nicht erreicht wurde.
	Dabei wird die eingelesene Zeile, anhand des verwendeten Trennzeichens in mehrere Bestandteile aufgeteilt,
	diese werden unteranderem mit der Hilfe der Typ-Konvertierungsfunktion std::stof dem Konstruktor der Dataset-Besucher-Klasse �bergeben,
	um einen Zeiger auf ein Dataset-Objekt zu erzeugen. Der besagte Zeiger wird dem �bergebenen Vektor f�r Zeiger auf Datesets hinzugef�gt. */
	while (std::getline(csvDatei, csvInhalt))
	{
		std::stringstream st(csvInhalt);
		std::string tmpString;
		std::vector<std::string> zeilenBestandteile;

		while (std::getline(st, tmpString, ';'))
			zeilenBestandteile.push_back(tmpString);

		Dataset* d = new Dataset(std::stof(zeilenBestandteile[0]), std::stof(zeilenBestandteile[1]));
		data.push_back(d);
	}
}

void erstelleCluster(std::vector<Dataset*>& data, float radius, int minPts)
{
	// wird f�r die Zuweisung eines Datensatzes in ein Cluster verwendet, bei jedem neu erzeugten Cluster, wird diese Variable inkrementiert
	int clusterID = 0;

	// Iteration �ber alle zu analysierenden Datens�tze
	for (int i = 0; i < data.size(); i++)
	{
		std::vector<std::vector<Dataset*>>erreichbareDatensaetze;
		std::vector<Dataset*> erreichbareDatensaetzeKernobjekt;

		// Wurde der Datensatz noch nicht besucht, erfolgt eine tiefergehende Betrachtung
		if (!data[i]->kontrolliereBesuch())
		{
			// Der zuvor noch nicht besuchte Datensatz, wird nun als besucht markiert
			data[i]->markiereAlsBesucht();


			/* Alle Datensaetze werden dahingehend untersucht, ob sie von dem aktuellen Objekt aus erreichbar sind 
			und ihre Distanz geringer als der Parameter "radius" ist*/
			for (int j = 0; j < data.size(); j++)
			{
				// Berechnung der euklidischen Distanz
				float abstand = sqrt((pow(data[i]->erhalteX() - data[j]->erhalteX(), 2) + pow(data[i]->erhalteY() - data[j]->erhalteY(), 2)));

				/* Sollte die berechnete Distanz geringer als der Grenzwert "radius" ausfallen, 
				wird der jeweilige Datensatz, dem Vektor "erreichbareDatensaetzeKernobjekt" hinzugef�gt */
				if (abstand < radius)
					erreichbareDatensaetzeKernobjekt.push_back(data[j]);
			}

			/* Sollte das zuvor potentielle Kernobjekt, mindestens so viele Datens�tze erreichen k�nnen, 
			wie durch den Parameter "minPts" vorgebeben wurde, so wird dieser dem Vektor "erreichbareDatensaetze" hinzugef�gt,
			und alle erreichbaren Datens�tze werden anschlie�end weitergehend auf wiederum erreichbare Datens�tze untersucht werden,
			sofern diese noch nicht besucht wurden, ist das jedoch nicht der Fall, wird der Datensatz als "Rauschen" identifiziert*/
			if (erreichbareDatensaetzeKernobjekt.size() >= minPts)
			{
				// Die ClusterID wird erh�ht, da es sich bei dem zuvor ausgew�hlten Datensatz wirklich um ein Kernobjekt handelt
				clusterID++;
				// Zuweisung des Kernobjekts in das neue Cluster
				data[i]->clusterzuweisung(clusterID);
				// Die Menge an erreichbaren Datens�tzen des Kernobjekts, wird dem Vektor erreichbareDatensaetze hinzugef�gt
				erreichbareDatensaetze.push_back(erreichbareDatensaetzeKernobjekt);

				// Alle Vektoren die dem Vektor f�r erreichbare Datens�tze hinzugef�gt wurden, werden untersucht
				for(int k = 0; k < erreichbareDatensaetze.size();k++)
				{
					// Iteration der einzelnen Datens�tze, in den jeweiligen Vektoren, die sich in dem Vektor "erreichbareDatensaetze" befinden
					for (int l = 0; l < erreichbareDatensaetze[k].size(); l++) 
					{
						Dataset* aktuellerDatensatz = erreichbareDatensaetze[k][l];

						/* �berpr�fung ob der aktuelle Datensatz bereits besucht wurde, 
						trifft dies zu, wird der aktuelle Datensatz nicht weitergehend ber�cksichtigt */
						if (!aktuellerDatensatz->kontrolliereBesuch()) 
						{
							// Speicherort f�r gefundene Datens�tze, die f�r den aktuellen Datensatz erreichbar sind
							std::vector<Dataset*> benachbarteDatensaetze;

							// Der aktuelle Datensatz wird als besucht markiert
							aktuellerDatensatz->markiereAlsBesucht();

							// Wurde der Datensatz noch keinem Cluster zugewiesen, wird er nun dem Cluster des entsprechenden Kernobjekts hinzugef�gt
							if (aktuellerDatensatz->erhalteClusterID() == 0)
								aktuellerDatensatz->clusterzuweisung(clusterID);

							// Alle Datens�tze werden dahingehend untersucht, welche Distanz sie zu dem aktuellen Datensatz besitzen
							for (int j = 0; j < data.size(); j++)
							{
								// Berechnung der euklidischen Distanz
								float abstand = sqrt((pow(aktuellerDatensatz->erhalteX() - data[j]->erhalteX(), 2) + pow(aktuellerDatensatz->erhalteY() - data[j]->erhalteY(), 2)));

								// Ist der Abstand geringer als der vorgegebene Grenzwert, wird er dem Vektor "benachbarteDatensaetze" hinzugef�gt
								if (abstand < radius)
									benachbarteDatensaetze.push_back(data[j]);
							}

							/* Besitzt der aktuelle Datensatz mindestens so viele Datenpunkte, wie durch den Parameter "minPts" vorgegeben,
							wird der Vektor "benachbarteDatensaetze" dem Vektor f�r erreichbare Datens�tze des Kernobjekts hinzugef�gt */
							if(benachbarteDatensaetze.size() >= minPts)
								erreichbareDatensaetze.push_back(benachbarteDatensaetze);

						}

					}
				}

			}
			else
				data[i]->clusterzuweisung(-1);
		}

	}
}

/* Die Funktion gibt die ermittelten Cluster, mit den Informationen �ber die Zoo-Besucher in einer CSV-Datei aus,
daf�r m�ssen die Datenstruktur, in welcher die Zoo-Besucher gespeichert wurden und eine Zeichenkette, f�r den Namen der CSV-Datei �bergeben werden. */
void erstelleCSVDatei(std::vector<Dataset*>& data, std::string dateiname)
{
	std::ofstream datei;
	datei.open(dateiname);

	datei << "X;Y;Cluster" << std::endl;

	for (int i = 0; i < data.size(); i++)
	{
		datei << data[i]->erhalteX() << ";" << data[i]->erhalteY() << ";" << data[i]->erhalteClusterID() << std::endl;
	}

	datei.close();
}

int main(int argc, char** argv) 
{
	std::vector<Dataset*> data;

	datenEinlesen(data, "Ausgangsdaten.csv");
	erstelleCluster(data, 0.05, 5);
	erstelleCSVDatei(data, "Ergebnis.csv");

	return EXIT_SUCCESS;
}
