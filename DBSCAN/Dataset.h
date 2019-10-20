#pragma once
class Dataset
{
private:
	// Attribute f�r die jeweiligen Koordinaten des Datensatzes
	float x_;
	float y_;

	// Kennzeichnet das zugewiesene Cluster des Datensatzes
	int clusterID_ = 0;

	// Kennzeichnet ob ein instanzieertes Objekt der Klasse bereits im Clusterverfahren besucht wurde
	bool bereitsBesucht_ = false;

public:
	// Konstruktor der Klasse
	Dataset(float x,float y) : x_{x},y_{y}{}

	// Geben bei Bedarf die jeweiligen Koordinaten eines Dataset-Objekts zur�ck
	float erhalteX() { return x_; };
	float erhalteY() { return y_; };

	// Zeigt auf ob ein Dataset-Objekt bereits besucht wurde
	bool kontrolliereBesuch() { return bereitsBesucht_; };

	// Gibt die ClusterID eines Dataset-Objekts zur�ck und zeigt damit dessen Clusterzugeh�rigkeit an
	int erhalteClusterID() { return clusterID_; };

	// Markiert ein Dataset-Objekt als bereits besucht
	void markiereAlsBesucht() { bereitsBesucht_ = true; };

	// �ndert die Clusterzugeh�rigkeit eines Dataset-Objekts
	void clusterzuweisung(int c) { clusterID_ = c; };

};

