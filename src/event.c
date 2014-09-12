#include <stdlib.h>
#include <gtk/gtk.h>
#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/simplemotion_private.h"
#include "SimpleMotion/vsd_cmd.h"
#include "constant.h"
#include "axis.h"
#include "gui.h"
#include "main.h"
#include "event.h"
#include "aux.h"

gint numericInputKeyPressEvent(GtkWidget *widget, gpointer userData) {
	//printf("Set Number!!\n");
	gtk_dialog_response(GTK_DIALOG(numberDialog), 1);

	//Actually set our data
	//TODO Scaling
	switch(InputType) {
	case INPUT_TYPE_TARGET:
		Target = (int)(atof(gtk_entry_get_text(GTK_ENTRY(numberEntry)))*CntPerInch);
		if(Target > TARGET_MAX) {Target = TARGET_MAX;}
		if(Target < TARGET_MIN) {Target = TARGET_MIN;}
		break;
	case INPUT_TYPE_FEEDRATE:
		Feedrate = (int)(atof(gtk_entry_get_text(GTK_ENTRY(numberEntry)))*VelPerIPM);
		if(Feedrate > FEEDRATE_MAX) {Feedrate = FEEDRATE_MAX;}
		if(Feedrate < FEEDRATE_MIN) {Feedrate = FEEDRATE_MIN;}
		break;
	case INPUT_TYPE_SPINDLE:
		Spindle = atoll(gtk_entry_get_text(GTK_ENTRY(numberEntry)));
		if(Spindle > SPINDLE_MAX) {Spindle = SPINDLE_MAX;}
		if(Spindle < SPINDLE_MIN) {Spindle = SPINDLE_MIN;}
		break;
	}
	InputType = INPUT_TYPE_NONE;
	return 0;
}

gint jogKey_press_event(GtkWidget *widget, GdkEventKey *event) {
	//Let's only send a new command to the drive if we change direction
	int previousJogDirection = JogDirection;

	//Only start a continous jog if not in a jog mode, or already in continous
	switch(JogMode) {
	case JOG_MODE_1X:
		//fallthrough
	case JOG_MODE_10X:
		//fallthrough
	case JOG_MODE_100X:
		switch(event->keyval) {
		//and a continous jog button has been pushed
		case GDK_KEY_KP_1:
			//Then jog
			JogDirection = JOG_FORWARD;
			break;
		case GDK_KEY_KP_3:
			JogDirection = JOG_REVERSE;
			break;
		
		//Other keys should also stop, jic
		default:
			//If moving, stop
			if(JOG_MODE_NONE != JogMode) {
				//printf("STOP DUE TO OTHER INPUT!!!!\n");
				JogDirection = JOG_STOP;
				State = STATE_STOP;
			}
			//Any of those cases, we want to return
			return 0;
			break;
		}
	
		if(JogDirection != previousJogDirection) {
			//Was a continous jog button, do the actual jog
			//printf("JOGGING!\n");
			smSetParam(AxisName, "ControlMode", CONTROL_MODE_VELOCITY);
			smSetParam(AxisName, "VelocityLimit", FEEDRATE_MAX);
			AxisStatus = smCommand(AxisName, "ABSTARGET", JogFeedrate[JogMode]*JogDirection);
		}

		break;
	}

	return 0;
}

gint jogKey_release_event(GtkWidget *widget, GdkEventKey *event) {
	//printf("JOG RELEASE KEY!: '%s'\n", gdk_keyval_name(event->keyval));
	
	if(JOG_STOP == JogDirection) {
		switch(event->keyval) {
		//Other keys first
		case GDK_KEY_Return:
			JogMode = JOG_MODE_NONE;
			gtk_dialog_response(GTK_DIALOG(jogDialog), 1);
			break;


		//Continuous Jog
		case GDK_KEY_KP_7:
			JogMode = JOG_MODE_1X;
			break;
		case GDK_KEY_KP_8:
			JogMode = JOG_MODE_10X;
			break;
		case GDK_KEY_KP_9:
			JogMode = JOG_MODE_100X;
			break;
		//Step Jogs
		//001
		case GDK_KEY_KP_4:
			JogMode = JOG_MODE_001;
			smSetParam(AxisName, "VelocityLimit", FEEDRATE_MAX);
			break;
		//01
		case GDK_KEY_KP_5:
			JogMode = JOG_MODE_01;
			smSetParam(AxisName, "VelocityLimit", FEEDRATE_MAX);
			break;
		//1
		case GDK_KEY_KP_6:
			JogMode = JOG_MODE_1;
			smSetParam(AxisName, "VelocityLimit", FEEDRATE_MAX);
			break;


		//Actual Jog buttons
		case GDK_KEY_KP_1:
			//fallthrough
		case GDK_KEY_KP_3:
			if(GDK_KEY_KP_1 == event->keyval) {JogDirection = JOG_FORWARD;}else{JogDirection = JOG_REVERSE;}

			switch(JogMode) {
			//Step Jogs
			case JOG_MODE_001:
				AxisStatus = smCommand(AxisName, "INCTARGET", JOG_FEEDAMOUNT_001*JogDirection);
				break;
			case JOG_MODE_01:
				AxisStatus = smCommand(AxisName, "INCTARGET", JOG_FEEDAMOUNT_01*JogDirection);
				break;
			case JOG_MODE_1:
				AxisStatus = smCommand(AxisName, "INCTARGET", JOG_FEEDAMOUNT_1*JogDirection);
				break;
			}
			JogDirection = JOG_STOP;
			break;
		}
	}else { //Already jogging, don't get stuck moving
		//printf("STOP!!!!\n");
		JogDirection = JOG_STOP;
		State = STATE_STOP;
	}

	//Highlight current mode
	int i;
	for(i = 0; i < 6; i++) {
		if(i == JogMode) {
			gtk_label_set_markup(GTK_LABEL(jogLabel[i]), g_markup_printf_escaped(jogLabelMarkup[i], "#000000"));
		}else {
			gtk_label_set_markup(GTK_LABEL(jogLabel[i]), g_markup_printf_escaped(jogLabelMarkup[i], "#AAAAAA"));
		}
	}

	return 0;
}

gint key_release_event(GtkWidget *widget, GdkEventKey *event) {
	//printf("KEY!: '%s'\n", gdk_keyval_name(event->keyval));
	switch(event->keyval) {
	//Emergency Retract
	case GDK_KEY_Escape:
		handleReset();
		break;
	//Nice End
	case GDK_KEY_F12:
		sigIntHandler();
		break;
	//Set Target
	case GDK_KEY_1:
		gtk_label_set_markup(GTK_LABEL(numberEntryLabel), g_markup_printf_escaped(numberEntryLabelMarkup, "Feed Amount"));
		if(STATE_IDLE == State) {
			InputType = INPUT_TYPE_TARGET;
			requestNumber();
		}
		break;
	//Set Feedrate
	case GDK_KEY_2:
		gtk_label_set_markup(GTK_LABEL(numberEntryLabel), g_markup_printf_escaped(numberEntryLabelMarkup, "Feedrate"));
		if(STATE_IDLE == State) {
			InputType = INPUT_TYPE_FEEDRATE;
			requestNumber();
		}
		break;
	//Set Spindle
	case GDK_KEY_3:
		gtk_label_set_markup(GTK_LABEL(numberEntryLabel), g_markup_printf_escaped(numberEntryLabelMarkup, "Spindle Speed"));
		if(STATE_IDLE == State) {
			InputType = INPUT_TYPE_SPINDLE;
			requestNumber();
		}
		break;
	case GDK_KEY_j:
		if(STATE_IDLE == State) {
			showJogDialog();
		}
		break;
	case GDK_KEY_space:
		if(STATE_IDLE == State) {
			State = STATE_START;
		}
		break;
	//Feedrate Override
	case GDK_KEY_KP_Divide:
		FeedrateOverride -= FEEDRATE_OVERRIDE_INC;
		if(FeedrateOverride < FEEDRATE_OVERRIDE_MIN) {FeedrateOverride = FEEDRATE_OVERRIDE_MIN;}
		break;
	case GDK_KEY_KP_Multiply:
		FeedrateOverride += FEEDRATE_OVERRIDE_INC;
		if(FeedrateOverride > FEEDRATE_OVERRIDE_MAX) {FeedrateOverride = FEEDRATE_OVERRIDE_MAX;}
		break;
	//Spindle Override
	case GDK_KEY_KP_Subtract:
		SpindleOverride -= SPINDLE_OVERRIDE_INC;
		if(SpindleOverride < SPINDLE_OVERRIDE_MIN) {SpindleOverride = SPINDLE_OVERRIDE_MIN;}
		break;
	case GDK_KEY_KP_Add:
		SpindleOverride += SPINDLE_OVERRIDE_INC;
		if(SpindleOverride > SPINDLE_OVERRIDE_MAX) {SpindleOverride = SPINDLE_OVERRIDE_MAX;}
		break;
	case GDK_KEY_z:
		break;
	}

	return 0;
}

gboolean updateDisplayEvent(gpointer userData) {
	updatePosition();
	updateDisplay();

	return TRUE;
}

gboolean updatePositionEvent(gpointer userData) {
	updatePosition();

	return TRUE;
}

gboolean updateStateEvent(gpointer userData) {
	doState();

	return TRUE;
}

