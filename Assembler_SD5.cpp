#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv) {

	int dalis;
	cout << "Kuria dali darom? (1 ar 2)" << endl;
	cin >> dalis;

	if (dalis == 1) {
		system("cls");
		cout << "Darom pikseliukus" << endl;
		string tempInput;
		ifstream inFile;
		inFile.open(argv[1]);
		inFile >> tempInput;

		char* pixel = new char[tempInput.size()];
		for (int i = 0; i < tempInput.size(); i++) pixel[i] = tempInput[i];

		int lengthSt = tempInput.size();
		int* rez;
		if (lengthSt > 1) rez = new int[lengthSt / 2];

		// PVZ.:
		// pixel[8] = { a, b, c, d, e, f, g, h }
		// ilgis elementais: 8
		// ilgis baitais: 8
		// 
		// rez[4] = { (a + b) / 2, (c + d) / 2, (e + f) / 2, (g + h) / 2 }
		// ilgis elementais: 4
		// ilgis baitais: 16


		__asm {

			PUSH eax
			PUSH ecx
			PUSH esi
			PUSH edi

			MOV ecx, lengthSt // ecx = lengthSt (pradinio masyvo elementu skaicius)
			AND ecx, 1		  // ecx(last bit) AND 1, tikrinamas ar ecx(lengthSt) lyginis ar nelyginis
			JNZ BADINPUT	  // jei paskutinis bitas 1 (nelyginis) jump to BADINPUT

			// pradinio CHAR masyvo pabaigos adreso formavimas:
			MOV eax, pixel	  // eax = pixel (pradinio CHAR masyvo pradzios adresas)
			MOV ecx, lengthSt // ecx = lengthSt (pradinio masyvo elementu skaicius)
			ADD eax, ecx	  // eax = pixel + lengthSt (pradinio CHAR masyvo pabaigos adresas + 1)
			MOV esi, eax	  // esi = eax (pradinio CHAR masyvo pabaigos adresas + 1)
			DEC esi			  // esi-- (pradinio CHAR masyvo pabaigos adresas)

			// rezultato INT masyvo pabaigos adreso formavimas:
			SHL ecx, 1	 // ecx (lengthSt) << 1 (lengthSt * 2, nes CHAR - 1 baitas ir x elementu, INT 4 baitai ir x/2 elementu)
			MOV eax, rez // eax = rez (rezultato INT masyvo pradzios adresas)
			ADD eax, ecx //	eax = rez + lengthSt * 2 (rezultato INT masyvo pabaigos adresas + 1)
			MOV edi, eax //	edi = eax (rezultato INT masyvo pabaigos adresas + 1)
			DEC edi		 // edi-- (rezultato INT masyvo pabaigos adresas)

			// rezultato INT masyvo nunulinimas, nuo pabaigos adreso:
			ZEROPREP :
				MOV[edi], 0  // rez[edi] = 0 
				DEC edi		 // edi-- (rezulato INT masyvo ADRESO mazinimas, nepasieks 0)
				DEC ecx		 // ecx-- (INT lengthSt * 2, pasieks 0)
				JNZ ZEROPREP // jei counteris ecx = 0, stop the ZEROPREP


			MOV ecx, lengthSt // ecx = lengthSt (pradinio masyvo elementu skaicius)
			SHR ecx, 1		  // ecx (lengthSt) >> 1 (lengthSt / 2, gaunamas rezultato masyvo INT elementu skaicius)
			MOV edi, eax	  // edi = eax (rezultato INT masyvo pabaigos adresas + 1)


			// gretimu pixeliu dalinimo, sudejimo ir idejimo i rezultato masyva ciklas:
			MERGING :
				// gretimi pixeliai idedami i al ir bl
				MOV al, [esi] // al (A LOW) = pixel[esi]
				DEC esi		  // esi--
				MOV bl, [esi] // bl (B LOW) = pixel[esi]
				DEC esi		  // esi--

				ADD al, bl	 // al = al + bl (sudedami pikseliu duomenys)
				SHR al, 1	  // al = al / 2 (pikesliu duomenys padalinami is 2)

				SUB edi, 4	 // mazinamas rezulato INT masyvo adresas (4, nes INT - 4 baitai, CHAR - 1 baitas)
				MOV[edi], al // rez[edi] = al (i rezultato INT masyva idedama dvieju gretimu pixeliu pusiu suma)
				DEC ecx		 // ecx-- (lengthSt / 2, rezultato masyvo INT elementu skaicius)
				JNZ MERGING  // jei counteris ecx = 0, stop the MERGING


				JMP QUIT // Jump to QUIT

			BADINPUT :
				MOV lengthSt, 0
				JMP QUIT

			QUIT :
				POP eax
				POP ecx
				POP esi
				POP edi

		}

		if (lengthSt == 0) {
			cout << "Paveiksleliu pikseliu kiekis nesutampa, t.y. CHAR masyvo ilgis yra nelyginis (bad input)" << endl;
			return 0;
		}
		cout << rez[0];
		for (int i = 1; i < lengthSt / 2; i++) cout << " " << rez[i];
	}

	if (dalis == 2) {
		system("cls");
		cout << "Darom Pitagora" << endl;
		ofstream outfile("rez.txt");
		outfile << "x,y,z" << endl;

		__declspec(align(16))float x[4];
		__declspec(align(16))float y[4];
		__declspec(align(16))float z[4];
		__declspec(align(16))int h[4];

		for (int i = 1; i < 1000; i += 4) {

			x[0] = i;
			x[1] = i + 1;
			x[2] = i + 2;
			x[3] = i + 3;

			for (int j = i + 1; j <= 1000; j++) {

				for (int g = 0; g < 4; g++) {
					y[g] = 2;
					if ((j + g) <= 1000) y[g] = j + g;
				}

				__asm {
					LEA eax, x
					MOVAPS xmm0, [eax]	// xmm0 == x masyvas

					LEA eax, y
					MOVAPS xmm1, [eax]	// xmm1 == y masyvas

					MULPS xmm0, xmm0	// x*x
					MULPS xmm1, xmm1	// y*y
					ADDPS xmm0, xmm1	// (x*x) + (y*y) = z*z
					SQRTPS xmm0, xmm0	// z

					LEA eax, z
					MOVAPS[eax], xmm0	// z masyvas == xmm0

					ROUNDPS xmm1, xmm0, 0	// apvalinamas z
					CMPEQPS xmm0, xmm1		// tikrinam ar z buvo sveikas ar crazy

					LEA eax, h
					MOVAPS[eax], xmm0	// rezultatu masyvas == z masyvas
				}

				for (int k = 0; k < 4; k++) {
					if (h[k] != 0) outfile << x[k] << " " << y[k] << " " << z[k] << endl;
				}
			}
		}
	}

}
