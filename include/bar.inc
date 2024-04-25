<?php
class Bar {
    private $x;
    private $width;
    private $dir;
    private $marks;
    private $lastScore;
    private $speed;

    public static $dirLeft = "LEFT";
    public static $dirRight = "RIGHT";

    public function __construct($width, $speed) {
        $this->x = 0;
        $this->width = $width;
        $this->speed = $speed;
        $this->dir = Bar::$dirRight;
        $this->marks = array();
    }

    public function getSpeed() {
        return $this->speed;
    }

    public function setSpeed($s) {
        $this->speed = $s;
    }
    
    public function setLastScore($ls) {
        $this->lastScore = $ls;
    }

    public function clearMarks() {
        $this->marks = array();
    }

    public function addMark() {
        $this->marks[] = $this->x;
    }

    public function getX() {
        return $this->x;
    }

    public function setX($x) {
        $this->x = $x;
    }

    public function switchDir() {
        if ($this->dir == Bar::$dirRight) {
            $this->dir = Bar::$dirLeft;
        } else {
            $this->dir = Bar::$dirRight;
        }
    }

    public function move() {
        if ($this->dir == Bar::$dirRight) {
            $this->moveRight();
        } else {
            $this->moveLeft();
        }
    }

    public function moveLeft() {
        if ($this->x > 0) {
            $this->x--;
        } else {
            $this->dir = Bar::$dirRight;
            $this->x++;
        }
    }

    public function moveRight() {
        if ($this->x < $this->width - 1) {
            $this->x++;
        } else {
            $this->dir = Bar::$dirLeft;
            $this->x--;
        }
    }

    public function getDir() {
        return $this->dir;
    }

    public function getWidth() {
        return $this->width;
    }

    public function tick() {
        $this->move();
    }

    public function draw() {
        $ret = "";

        for ($x = 0; $x < $this->width; $x++) {
            if ($x == $this->x) {
                $ret .= "|";
            } else if (in_array($x, $this->marks)) {
                $ret .= "m";
            } else {
                $ret .= "-";
            }
        }

        if (!is_null($this->lastScore)) {
            $ret .= " [" . $this->lastScore . "%]";
        }

        $ret .= "\n";

        return $ret;
    }
}
