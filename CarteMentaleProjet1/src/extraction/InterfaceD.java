package extraction;

import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class InterfaceD extends JFrame{
	
	private JButton bouton;
 
	public InterfaceD(){
		super();
 
		build();//On initialise notre fenêtre
	}
 
	private void build(){
		setTitle("Interface du Configuration"); //On donne un titre à l'application
		setSize(600,300); //On donne une taille à notre fenêtre
		setLocationRelativeTo(null); //On centre la fenêtre sur l'écran
		//setResizable(false); //On interdit la redimensionnement de la fenêtre
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); //On dit à l'application de se fermer lors du clic sur la croix
		setContentPane(buildContentPane());
	}
	
	
	private ab JPanel buildContentPane(){
		JPanel panel = new JPanel();
		panel.setLayout(new FlowLayout());
		panel.setBackground(Color.white);
	 
		JLabel label = new JLabel("Bienvenue dans l'application ");
		
		JButton bouton = new JButton("Cliquez ici !");
		panel.add(bouton);
		//bouton.addActionListener(this);
	 
		panel.add(label);
	 
		return panel;
	}	
	
	
	public void actionPerformed(ActionEvent e) {
		Object source = e.getSource();
 
		if(source == bouton){
			System.out.println("Vous avez cliqué ici.");}
	
	}
	
	/*
	
	public static void main(){
		
	}*/
}