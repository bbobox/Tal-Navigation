package extraction;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import fr.umlv.unitex.jni.UnitexJni;

class TestUnitex {
	
	
	private String nomdocument;
	
	private String nomBrut;
	
	public TestUnitex(String doc){
		
		
		nomdocument=doc;
		nomBrut=doc;
		Pattern p= Pattern.compile("(.*)[\".][a-zA-Z]*");
		Matcher m= p.matcher(nomdocument);
		if (m.find()){
			nomdocument=(m.group(1));
		}
	}
	
	public void sortieUnitex(){
		File file = new File(nomdocument+"_snt");
		file.mkdir();
		
		File filexml = new File(nomdocument+"_xml");
		filexml.mkdir();
		
		String persistentAlphabet = UnitexJni.loadPersistentAlphabet("French/Alphabet.txt");
		
		/* Partie Preprocessing */
			/* 1- Application de la normalisation sur le document : Normalize  */
		UnitexJni.execUnitexTool( "UnitexTool Normalize "+nomBrut+" -rFrench/Norm.txt \"--output_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom");
		
			/* 2- Application Sequençage du document */
				/*-Complilation de la Grammaire */
		UnitexJni.execUnitexTool( "UnitexTool Grf2Fst2 \"French/Graphs/Preprocessing/Sentence/Sentence.grf\" -y \"-aFrench/Alphabet.txt\" -qutf8-no-bom");
				/* Transformation en transducteur à etats finis: Flatten */
		UnitexJni.execUnitexTool("UnitexTool Flatten \"French/Graphs/Preprocessing/Sentence/Sentence.fst2\" --rtn \"-d5\" -qutf8-no-bom");
				/* Application du Transducteur au texte en phase de pretraitement : Fst2Text*/
		UnitexJni.execUnitexTool("UnitexTool Fst2Txt \"-t"+nomdocument+".snt\" French/Graphs/Preprocessing/Sentence/Sentence.fst2 \"-aFrench/Alphabet.txt\" -M \"--input_offsets="+nomdocument+"_snt/normalize.out.offsets\" --output_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom"); 
		UnitexJni.execUnitexTool( "UnitexTool Grf2Fst2 \"French/Graphs/Preprocessing/Replace/Replace.grf\" -y \"-aFrench/Alphabet.txt\" -qutf8-no-bom");
		UnitexJni.execUnitexTool("UnitexTool Fst2Txt \"-t"+nomdocument+".snt\" French/Graphs/Preprocessing/Replace/Replace.fst2 \"-aFrench/Alphabet.txt\" -R \"--input_offsets="+nomdocument+"_snt/normalize.out.offsets\" --output_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom"); 
		
		/* Ce programme découpe le texte en unités lexicales : Tokenize */
		
		UnitexJni.execUnitexTool("UnitexTool Tokenize \""+nomdocument+".snt\" -aFrench/Alphabet.txt \"--input_offsets="+nomdocument+"_snt/normalize.out.offsets\" --output_offsets="+nomdocument+"_snt/tokenize.out.offsets\" -qutf8-no-bom");
		
		
		/* Application de Dictionnaire au Texte ( deja decoupé par la fonction Tokenize) :*/
	    UnitexJni.execUnitexTool("UnitexTool Dico \"-t"+nomdocument+".snt\" -aFrench/Alphabet.txt \"French/Dela/dela-fr-public.bin\" French/Dela/ajouts80jours.bin \"French/Dela/motsGramf-.bin\" -qutf8-no-bom");
	    
	    /* Tri Lexicographiqye des  lignes du fichier Fichier S: SortTxt */
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/dlf\" -l"+nomdocument+"_snt/dlf.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/dlc\" -l"+nomdocument+"_snt/dlc.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/err\" -l"+nomdocument+"_snt/err.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/tags_err\" -l"+nomdocument+"_snt/tags_err.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    
	    
	    /* Partie Recherche de Relation Semantique dans le Document */
			    
		UnitexJni.execUnitexTool("UnitexTool Cassys \"-aFrench/Alphabet.txt\" -lFrench/CasSys/cass.csc \"-t"+nomdocument+".snt\" -v \"-rFrench/Graphs/\" --input_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom");
		
		//production de la concordance */
		
		UnitexJni.execUnitexTool("UnitexTool Concord \""+nomdocument+"_snt/concord.ind\" -l40 -r55 \"--CL\" --merge=resText.SNT \"-aFrench/Alphabet.txt");
		
		
		/*--------------------------------------------------------------------------------------------------*/
		
		
		System.out.println("--- Fin de la Partite UNITEX");
		System.out.println("--- Copie du fichier snt UNITEX");
		
		try {
			BufferedWriter script1= new BufferedWriter(new FileWriter("testscript.sh"));
			
			script1.write("#!/bin/bash"); script1.newLine(); script1.newLine();
			script1.write("iconv "+nomdocument+".snt -f utf16 -t utf8  > "+nomdocument+"Conv.txt"); script1.newLine(); script1.newLine();
			script1.write("exit 0");
			script1.close();
			
			
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		 String[] cmdText={"iconv",nomdocument+".snt","-f","utf16","-t","utf8"};
		 String[] cmdText2={"./testscript.sh"};
		 try {
			Process procConv=Runtime.getRuntime().exec(cmdText2);
			try {
				procConv.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			/*try {
				procConv.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
		 
		 
		/* //File concordDirectory = new File(nomdocument+"_snt");
		 try {
			Process procConvert=Runtime.getRuntime().exec(cmdText);
			procConvert.waitFor();
			BufferedReader buf = new BufferedReader(new InputStreamReader(procConvert.getInputStream()));
			
			BufferedWriter sortieConv = new BufferedWriter(new FileWriter(nomdocument+"Conv.txt"));
			 
			 String line = "";
			 int i=0;
			 while ((line=buf.readLine())!=null) {
				 i++;
			
				 sortieConv.write(line);sortieConv.newLine();
				 }
			 sortieConv.close();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
		
		 System.out.println("--- Fin de Copie du fichier SNT UNITEX------------");
		 
		 System.out.println("--- Recherche d'ENTITE NOMME AVEC TAGEN------------");
		
		//ETAPE 2: Utilisation de TagEN pour la reconnaissance des entite Nommées 
		 
		 File ftag = new File(nomdocument+"Conv.tag.txt");
		 if (ftag.exists()){
			 ftag.delete();
			// ftag.deleteOnExit();
		 }
		 
		 
		 try {
				BufferedWriter script2= new BufferedWriter(new FileWriter("scriptagEN.sh"));
				
				script2.write("#!/bin/bash"); script2.newLine(); script2.newLine();
				script2.write("cd TagEN ; ./tagen :mucfr ../"+nomdocument+"Conv.txt"); script2.newLine(); script2.newLine();
				script2.write("exit 0");
				script2.close();
				
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		
		// String[] cmd= {"./tagen",":mucfr","../"+nomdocument+"Conv.txt"};
		 String[] cmd= {"./scriptagEN.sh"};
		 File dir = new File("TagEN");
		 try {
			//Process proc=Runtime.getRuntime().exec(cmd,null,dir);
			 Process proc=Runtime.getRuntime().exec(cmd);
			 try {
				proc.waitFor();
				//ConstructionListeEntite fichierListeEntite= new ConstructionListeEntite(nomdocument+"Conv.tag.txt",nomdocument+"ListeEntite.txt");
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		 }
		 
		 System.out.println("--- fin de recherche  d'ENTITE NOMME AVEC TAGEN------------");
		 
		 
		 /* Utilasation de TreeTagger Pour L'etiquetage Morpho Synthaxique */
		 
		
		 System.out.println("--- Script Tree Tagger------------");
		 
		 try {
				BufferedWriter script3= new BufferedWriter(new FileWriter("ttgscript.sh"));
				
				script3.write("#!/bin/bash"); script3.newLine(); script3.newLine();
				script3.write("cd treetagger ; sh cmd/tree-tagger-french ../"+nomdocument+".txt > ../"+nomdocument+".ttg ;"); script3.newLine(); script3.newLine();
				script3.write("exit 0");
				script3.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		 String[] cmdTtg= { "./ttgscript.sh"};
		 try {
			Process procttg=Runtime.getRuntime().exec(cmdTtg);
			procttg.waitFor();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		 System.out.println("--- Fin Script TreeTagger------------");
		 
		 /*
		 File directory = new File("treetagger");
		 try {
			Process proc=Runtime.getRuntime().exec(cmdTtg,null,directory);
			proc.waitFor();
			BufferedReader buf = new BufferedReader(new InputStreamReader(proc.getInputStream()));
			
			BufferedWriter sortiettg = new BufferedWriter(new FileWriter(nomdocument+".ttg"));
			 
			 String line = "";
			 int i=0;
			 while ((line=buf.readLine())!=null) {
				 i++;
			
				 sortiettg.write(line);sortiettg.newLine();
				 }
			 sortiettg.close();
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		 } catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		 */
		 // Extration des Termes avec YateA*****************************************************************************************/
		 
		 //String[] cmdYateA= { "yatea","-rcfile","/usr/local/etc/yatea/yatea.rc",nomdocument+".ttg"};
		 
		 System.out.println("--- Script YateA------------");
		 
		 try {
				BufferedWriter script4= new BufferedWriter(new FileWriter("yateaScript.sh"));
				
				script4.write("#!/bin/bash"); script4.newLine(); script4.newLine();
				script4.write("yatea -rcfile /usr/local/etc/yatea/yatea.rc "+nomdocument+".ttg ; iconv yatea.dtd > "+nomdocument+"/default/xml/yatea.dtd ;"); script4.newLine(); script4.newLine();
				script4.write("exit 0");
				script4.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		 
		 
		 String[] cmdYateA={"./yateaScript.sh"};
		 try {
			Process proc=Runtime.getRuntime().exec(cmdYateA);	
			try {
				proc.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			 //String[] cpy={"iconv","yatea.dtd"};
			 //File concordDirectory = new File(nomdocument+"_snt");
			/* try {
				Process procConvert=Runtime.getRuntime().exec(cpy);
				procConvert.waitFor();
				BufferedReader buf = new BufferedReader(new InputStreamReader(procConvert.getInputStream()));
				
				BufferedWriter sortieConv = new BufferedWriter(new FileWriter(nomdocument+"/default/xml/yatea.dtd"));
				 String line = "";
				 int i=0;
				 while ((line=buf.readLine())!=null) {
					 i++;
				
					 sortieConv.write(line);sortieConv.newLine();
					 }
				 sortieConv.close();
				
			/*} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();} 
		 
		// TestJDom T= new TestJDom(nomdocument+"/default/xml/candidates.xml",nomdocument+"TermesYatea.txt"); 
		// T.listeTermes();
		 
		 /*
		 
		 String[] cpy={"iconv","yatea.dtd"};
		 //File concordDirectory = new File(nomdocument+"_snt");
		 try {
			Process procConvert=Runtime.getRuntime().exec(cpy);
			procConvert.waitFor();
			BufferedReader buf = new BufferedReader(new InputStreamReader(procConvert.getInputStream()));
			
			BufferedWriter sortieConv = new BufferedWriter(new FileWriter("Boka/yatea.dtd"));
			 String line = "";
			 int i=0;
			 while ((line=buf.readLine())!=null) {
				 i++;
			
				 sortieConv.write(line);sortieConv.newLine();
				 }
			 sortieConv.close();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
		 
		 
		  
		 /*String[] cmdcpy= { "cp","yatea.dtd",nomdocument+"/default/xml/"};
		 try {
			Process proc=Runtime.getRuntime().exec(cmdcpy);			
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();}*/
		 
		 
		 // Construction du XML des Termes
		 System.out.println("--- Copie des Concordances------------");
		 
		 
		 TestJDom T= new TestJDom(nomdocument+"/default/xml/candidates.xml",nomdocument+"TermesYatea.txt"); 
		 T.listeTermes();
		 
		 try {
				BufferedWriter script5= new BufferedWriter(new FileWriter("concordScript.sh"));
				
				script5.write("#!/bin/bash"); script5.newLine(); script5.newLine();
				script5.write("cd "+nomdocument+"_snt ; iconv concord.ind -t utf8 -f utf16 > ../"+nomdocument+"_concord.txt"); script5.newLine(); script5.newLine();
				script5.write("exit 0");
				script5.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		 String[] cmdConcordance={"./concordScript.sh"};
		 //File concordDirectory = new File(nomdocument+"_snt");
		 try {
			//Process procConvert=Runtime.getRuntime().exec(cmdConcordance,null,concordDirectory);
			 Process procConvert=Runtime.getRuntime().exec(cmdConcordance); 
			procConvert.waitFor();
			//BufferedReader buf = new BufferedReader(new InputStreamReader(procConvert.getInputStream()));
			
			//BufferedWriter sortieConcord = new BufferedWriter(new FileWriter(nomdocument+"_concord.txt"));
			 
			// String line = "";
			// int i=0;
			// while ((line=buf.readLine())!=null) {
			//	 i++;
			
			//	 sortieConcord.write(line);sortieConcord.newLine();
				// }
		//	 sortieConcord.close();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		 
		 System.out.println("--- Listage des Termes------------");
		 ExtractionTermes eT= new ExtractionTermes(nomdocument+"TermesYatea.txt",nomdocument+"_concord.txt",nomdocument+"CompletTermes.txt");
		 eT.productionFichier();
		 
		// iconv Boka.snt -f utf16 -t utf8
		 System.out.println("---  COnstruction du XML des TERMES------------");

		 IndexationsTermes ITermes= new IndexationsTermes(nomdocument+"CompletTermes.txtAlpha.txt",nomdocument+"Conv.txt");
		 ITermes.indexTermes();
		 ITermes.xmlTermes(nomdocument+"_xml/"+nomdocument+"_XmlTermes.xml");
		 
		 System.out.println("---  fin de  COnstruction du XML des TERMES------------");
		 
		 // Construction du XML des Phrases 
		 
		 XmlSequence doc= new XmlSequence(nomdocument+"_xml/"+nomdocument+"_XmlPhrases.Xml",nomdocument+"Conv.txt");
		try {
			doc.conctuctionXml();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		
		 // Construction du XML des Relations
		 try {
				BufferedWriter script6= new BufferedWriter(new FileWriter("scriptCascade.sh"));
				
				script6.write("#!/bin/bash"); script6.newLine(); script6.newLine();
				script6.write("iconv "+nomdocument+"_csc.txt -f utf16 -t utf8 > "+nomdocument+"_Cascade.txt"); script6.newLine(); script6.newLine();
				script6.write("exit 0");
				script6.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		
		
		//String[] cmdIdentConcord={"iconv",nomdocument+"_csc.txt","-f","utf16","-t","utf8"};
		String[] cmdIdentConcord={"./scriptCascade.sh"};
		 try {
			Process procCSC=Runtime.getRuntime().exec(cmdIdentConcord);
			procCSC.waitFor();
			/*BufferedReader buf = new BufferedReader(new InputStreamReader(procCSC.getInputStream()));
			
			BufferedWriter sortieConv = new BufferedWriter(new FileWriter(nomdocument+"_Cascade.txt"));
			 
			 String line = "";
			 int i=0;
			 while ((line=buf.readLine())!=null) {
				 i++;
			
				 sortieConv.write(line);sortieConv.newLine();
				 }
			 sortieConv.close();*/
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		 System.out.println("---  Construction Relation Semanqtique------------"); 
		
		IndexationRSemantiques rS= new IndexationRSemantiques(nomdocument+"_concord.txt",ITermes.getEnsembleTermes(),nomdocument+"_Cascade.txt");
		rS.xmlRSmenatiques(nomdocument+"_xml/"+nomdocument+"_XmlRelationsSemantiques.xml"); 
		
		 System.out.println("---  Fin Construction Relation Semantiques------------"); 
		
		
		// Construction du XMl des Entite Nommées 
		 
		 System.out.println("---  Construction Entite Nommées------------"); 
		
		ConstructionListeEntite fichierListeEntite= new ConstructionListeEntite(nomdocument+"Conv.tag.txt",nomdocument+"ListeEntite.txt");
		
		 
		
		IndexationsEntiteN IEntiteN= new IndexationsEntiteN(nomdocument+"ListeEntite.txt",nomdocument+"_Cascade.txt",nomdocument+"CompletTermes.txtAlpha.txt",nomdocument+"Conv.tag.txt");
		IEntiteN.IndexationsEntiteN();
		IEntiteN.XmlENtiteN(nomdocument+"_xml/"+nomdocument+"_XmlEntiteN.xml");
		
		
		// Construction du Xml general du fichier 
		
		 System.out.println("--- Fin de construction Entité------------"); 
		 
		 System.out.println("---  Construction XML DU DOCUMENT------------"); 
		 
		XmlDoc d= new XmlDoc(nomdocument+"_xml/"+nomdocument+".xml",nomdocument+"_xml/"+nomdocument+"_XmlPhrases.Xml",
				nomdocument+"_xml/"+nomdocument+"_XmlTermes.xml",nomdocument+"_xml/"+nomdocument+"_XmlEntiteN.xml",
				nomdocument+"_xml/"+nomdocument+"_XmlRelationsSemantiques.xml");
		
		
		 
		
	}
	
		/*

		public static void main(String[] args){
			Date dStart = new Date();
			TestUnitex tU= new TestUnitex("testYatea.txt"); 
			tU.sortieUnitex();
			
			Date dStop = new Date();
			System.out.println("Temps total d'excution : " + (dStop.getTime() - dStart.getTime() + " "));
			//tU.sortieUnitex();
			//Thread a = new Thread();
			//a.run();
			//tU.sortieUnitex();
			//tU.sortieUnitex();
			//tU.sortieUnitex();
			//Thread a = new Thread();
			
			
			
			
			/* String[] cmdTtg= { "sh","cmd/tree-tagger-french","../Boka.txt"};
			 File directory = new File("treetagger");
			 try {
				Process proc=Runtime.getRuntime().exec(cmdTtg,null,directory);
				proc.waitFor();
				BufferedReader buf = new BufferedReader(new InputStreamReader(proc.getInputStream()));
				
				BufferedWriter sortiettg = new BufferedWriter(new FileWriter("Boka.ttg"));
				 
				 String line = "";
				 int i=0;
				 while ((line=buf.readLine())!=null) {
					 i++;
				
					 sortiettg.write(line);sortiettg.newLine();
					 }
				 sortiettg.close();
			 } catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			 } catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/
			
			
			
			
			//Copie du fichier de sortie 
			
			
			/* Extraction des Termes avec Yatea */
			
			
			
			//System.out.println("aaaaa");
			//int a=UnitexJni.execUnitexTool("UnitexTool Normalize \"text.Unitex.txt\" -r \"Norm.txt\"");
			
			
			/* Test de cassys*/
			//String persistentFst2 = UnitexJni.loadPersistentFst2("definir.fst2");
			//String persistentAlphabet = UnitexJni.loadPersistentAlphabet("French/Alphabet.txt");
			//System.out.println(persistentFst2);
			
			/* Partie Preprocessing */
				//File file = new File("text.Unitex_snt");
				//file.mkdir();
				/* 1- Application de la normalisation sur le document : Normalize  */
				//UnitexJni.execUnitexTool( "UnitexTool Normalize \"text.Unitex.txt\" -rFrench/Norm.txt \"--output_offsets=text.Unitex_snt/normalize.out.offsets\" -qutf8-no-bom");
			
				/* 2- Application Sequençage du document */
					/*-Complilation de la Grammaire */
					//UnitexJni.execUnitexTool( "UnitexTool Grf2Fst2 \"French/Graphs/Preprocessing/Sentence/Sentence.grf\" -y \"-aFrench/Alphabet.txt\" -qutf8-no-bom");
					
					/* Transformation en transducteur à etats finis: Flatten */
					//UnitexJni.execUnitexTool("UnitexTool Flatten \"French/Graphs/Preprocessing/Sentence/Sentence.fst2\" --rtn \"-d5\" -qutf8-no-bom");
					
					/* Application du Transducteur au texte en phase de pretraitement : Fst2Text*/
					//UnitexJni.execUnitexTool("UnitexTool Fst2Txt \"-ttext.Unitex.snt\" French/Graphs/Preprocessing/Sentence/Sentence.fst2 \"-aFrench/Alphabet.txt\" -M \"--input_offsets=text.Unitex_snt/normalize.out.offsets\" --output_offsets=text.Unitex_snt/normalize.out.offsets\" -qutf8-no-bom"); 
					
				  
				
					//UnitexJni.execUnitexTool( "UnitexTool Grf2Fst2 \"French/Graphs/Preprocessing/Replace/Replace.grf\" -y \"-aFrench/Alphabet.txt\" -qutf8-no-bom");
					
					//UnitexJni.execUnitexTool("UnitexTool Fst2Txt \"-ttext.Unitex.snt\" French/Graphs/Preprocessing/Replace/Replace.fst2 \"-aFrench/Alphabet.txt\" -R \"--input_offsets=text.Unitex_snt/normalize.out.offsets\" --output_offsets=text.Unitex_snt/normalize.out.offsets\" -qutf8-no-bom"); 
					
					
					/* Ce programme découpe le texte en unités lexicales : Tokenize */
				
					//UnitexJni.execUnitexTool("UnitexTool Tokenize \"text.Unitex.snt\" -aFrench/Alphabet.txt \"--input_offsets=text.Unitex_snt/normalize.out.offsets\" --output_offsets=text.Unitex_snt/tokenize.out.offsets\" -qutf8-no-bom");
					
					
					/* Application de Dictionnaire au Texte ( deja decoupé par la fonction Tokenize) :*/
				    //UnitexJni.execUnitexTool("UnitexTool Dico \"-ttext.Unitex.snt\" -aFrench/Alphabet.txt \"French/Dela/dela-fr-public.bin\" French/Dela/ajouts80jours.bin \"French/Dela/motsGramf-.bin\" -qutf8-no-bom"); 
					
				    /* Tri Lexicographiqye des  lignes du fichier Fichier S: SortTxt */
				    
				    //UnitexJni.execUnitexTool("UnitexTool SortTxt \"text.Unitex_snt/dlf\" -ltext.Unitex_snt/dlf.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
				    
				    //UnitexJni.execUnitexTool("UnitexTool SortTxt \"text.Unitex_snt/dlc\" -ltext.Unitex_snt/dlc.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
				    
				    //UnitexJni.execUnitexTool("UnitexTool SortTxt \"text.Unitex_snt/err\" -ltext.Unitex_snt/err.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
				    
				    //UnitexJni.execUnitexTool("UnitexTool SortTxt \"text.Unitex_snt/tags_err\" -ltext.Unitex_snt/tags_err.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
				    
				    
		    /* Partie Recherche de Relation Semantique dans le Document */
				    
				   //UnitexJni.execUnitexTool("UnitexTool Cassys \"-aFrench/Alphabet.txt\" -lFrench/CasSys/cass.csc \"-ttext.Unitex.snt\" -v \"-rFrench/Graphs/\" --input_offsets=text.Unitex_snt/normalize.out.offsets\" -qutf8-no-bom");
			
			//String concord="ext.Unitex"+"ext.Unitex_csc/ext.Unitex_1_0_snt/concord.ind";
			//production de la concordance */
			
				//UnitexJni.execUnitexTool("UnitexTool Concord \"text.Unitex_snt/concord.ind\" -l40 -r55 \"--CL\" --merge=resText.SNT \"-aFrench/Alphabet.txt");
					
		 //System.out.println("fait------------"); //  -fCourier new\" -s12 -l40 -r55 fCourier New\" -s12\" -l40 -r40 --TO --html Si tu veux, 
		 
		 
		
		
		/* 
		 if (aa.find()){
			 System.out.println("trouvé");
		 }*/
		 
		 
		// int l=t.length();
		// System.out.println(l);
		 
		/* String[] tableau={"ls",">","titi"};
		 try {
			Process proc=Runtime.getRuntime().exec("gedit");
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		 	}*/
		 
		/* HashSet monHashSet=new HashSet(); // on crée notre Set
		 monHashSet.add(new String("1")); // on ajoute des string quelconques
		 monHashSet.add(new String("2"));
		 monHashSet.add(new String("3"));
		 monHashSet.add(new String("1")); // oups, je l'ai déja ajouté, la fonction gère l'exception levée, et l'objet n'est pas ajouté
		 Iterator i=monHashSet.iterator(); // on crée un Iterator pour parcourir notre HashSet
		 while(i.hasNext()) // tant qu'on a un suivant
		 {
		 	System.out.println(i.next()); // on affiche le suivant
		 }*/
		// String cmd = "ls";

		/* Runtime run = Runtime.getRuntime();

		 Process pr;
		try {
			pr = run.exec(cmd);
			
			
			
			 pr.waitFor();
			 
			 BufferedReader buf = new BufferedReader(new InputStreamReader(pr.getInputStream()));
			 
			 String line = "";

			 while ((line=buf.readLine())!=null) {

			 System.out.println(line);

			 }
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/


//		 BufferedReader buf = new BufferedReader(new InputStreamReader(pr.getInputStream()));

		
		 
		 
	//	}

}